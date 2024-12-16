# The scripts is a part of Metadata enablement for Okulo C1 devices on Windows10 OS.
#
# The flow is summarized as follows
# Find all the connected Okulo C1 devices
# For each device that supports metadata
#	Check whether the Metadata registry key is present
#	Add the missing key(s) if necessary
# Additionally the script can retrofit the metadata keys for all devices that were registered, but not necessarily connected
# Usage:
#Launch Windows PowerShell, and navigate to the script directory
#PS> .\okulo_c1_metadata_win.ps1                    - Add metadata reg. keys for the connected Okulo C1 devices
#PS> .\okulo_c1_metadata_win.ps1 -op install        - See above
#PS> .\okulo_c1_metadata_win.ps1 -op install_all    - Add metadata reg. keys for all Okulo C1 devices that were previously connected
#PS> .\okulo_c1_metadata_win.ps1 -op remove         - Remove metadata reg. keys for the connected Okulo C1 devices
#PS> .\okulo_c1_metadata_win.ps1 -op remove_all     - Remove metadata reg. keys for all Okulo C1 devices that were previously connected

#Parse command-line argument
param (
    [string]$op = "install"
)

# Elevate to admin - https://stackoverflow.com/questions/7690994/powershell-running-a-command-as-administrator
If (-NOT ([Security.Principal.WindowsPrincipal][Security.Principal.WindowsIdentity]::GetCurrent()).IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator))
{
    # The original script is modified to pass through the command-line parameter
    $arguments = "& '" + $myinvocation.mycommand.definition + "'" + " -op " + $op
    Start-Process powershell -Verb runAs -ArgumentList $arguments
    Break
}

# Base location for the registry keys we need to add
$DevConfigRegBase = "HKLM:\SYSTEM\CurrentControlSet\Control\DeviceClasses"
# The Registry subtrees for the Metadata keys shall be added
$guid1 = "{e5323777-f976-4f5b-9b55-b94699c46e44}";
$guid2 = "{65E8773D-8F56-11D0-A3B9-00A0C9223196}";

$SearchTrees = "$DevConfigRegBase\$guid1", "$DevConfigRegBase\$guid2"

# Multipin devices that need additional key MetadataBufferSizeInKB1
$MultiPinDevices =
    "USB\VID_367A&PID_C101&MI_00" # OkuloC1

#Inhibit system warnings and erros, such as permissions or missing values
$ErrorActionPreference = "silentlycontinue"

$ConnectedDev = @()
#Retrieve all connected UVC devices
$DevInReg = Get-ChildItem hklm:\SYSTEM\CurrentControlSet\Services\usbvideo | ForEach-Object {Get-ItemProperty $_.pspath}

#Transform output into a standard container
for ($i=0; $i -lt $DevInReg[0].Count; $i++) { $ConnectedDev +=$DevInReg[0].$i}

#Filter Okulo C1 devices
$ConnectedDev = $ConnectedDev -like "*VID_367A*"

#Progress notification
$rs_count = $ConnectedDev.Count
echo "$rs_count connected Okulo C1 devices were found:" $ConnectedDev

#Search each subtree for values that correspond to the requested Okulo C1 devices
foreach ($subtree in $SearchTrees)
{
    "`nProcessing Registry branch $subtree"
    #Get records for all UVC devices records
    $Items = Get-ChildItem $subtree | Foreach-Object {Get-ItemProperty $_.PsPath }

    #Filter Okulo C1 devices
    "There are " + $Items.Count +" total devices listed"
    $Items = $Items | Where { $_.DeviceInstance -like "*VID_367A*" }
    "" + $Items.Count + " of them are Okulo C1"

    $remove_keys = 0
    switch ($op)
    {
        "install"     { $Items =  $Items | Where { $ConnectedDev -contains $_.DeviceInstance }}
        "remove"      { $Items =  $Items | Where { $ConnectedDev -contains $_.DeviceInstance }; $remove_keys = 1 }
        "install_all" { }
        "remove_all"  { $remove_keys = 1 }
        default       { "Aborting: unrecognized argument "" + $op + "" provided.`nPossible values are:";
                        "`t`t -op [install/install_all/remove/remove_all].`nRefer to the installation manual for details"; Sleep 2; Exit }
    }

    foreach  ($item in $Items)
    {
        $fullPath = $item.PSPath+'\#global\Device Parameters'

        if ($remove_keys -ge 1)
        {
            "Remove keys for device: " + $item.DeviceInstance.ToString()
            # Non-present value will be ignored as for script execution policy
            Remove-ItemProperty -path $fullPath -name MetadataBufferSizeInKB0
            Remove-ItemProperty -path $fullPath -name MetadataBufferSizeInKB1
        }
        else
        {
            $val = 0,0,0
            $val[0] = Get-ItemPropertyValue -Path $fullPath -Name MetadataBufferSizeInKB0
            $val[1] = Get-ItemPropertyValue -Path $fullPath -Name MetadataBufferSizeInKB1

            if ($val[0] -eq 0)
            {
                "Device " +  $item.DeviceInstance.ToString() + ": adding metadata key"
                Set-ItemProperty -path $fullPath -name MetadataBufferSizeInKB0 -value 5
            }
            else
            {
                "Device " +  $item.DeviceInstance.ToString() + ": skiping - metadata key already exists"
            }

            if (($MultiPinDevices -contains $item.DeviceInstance.Substring(0,27)) -and ($val[1] -eq 0))
            {
                # Multi-pin interface requires an additional key
                "Device " +  $item.DeviceInstance.ToString() +": adding extra key for multipin interface"
                Set-ItemProperty -path $fullPath -name MetadataBufferSizeInKB1 -value 5
            }
        }
    }
}

"`nTask Completed"
sleep 1
