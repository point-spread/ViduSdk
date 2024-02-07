$PathVal = $args[0]
$LABEL = $args[1]
$VARIABLE = $args[2]
$Level = $args[3]

$SYSTEM_LEVEL = 'user'
if ($Level -eq 'true')
{
    $SYSTEM_LEVEL = 'machine'
}

$TempPath = [System.Environment]::GetEnvironmentVariable($VARIABLE, $SYSTEM_LEVEL)
if ([string]::IsNullOrEmpty($TempPath))
{
    $TempPath=$PathVal
}
$TempPath = $TempPath.Replace("/", "\")

# Write-Host "TempPath" $TempPath
$LABELS = ($LABEL.Split(";") | Group-Object | Where-Object {-not [string]::IsNullOrEmpty($_.Name)}).Values;

$Pre = ($TempPath.Split(";") | Group-Object | Where-Object {$SplitName = $_.Name; $LABELS | ForEach-Object{if ($SplitName.Contains($_)) {return 1;}}; return 0;}).Values;
if ($Pre.Count -gt 0) {$Pre | ForEach-Object {if (-not [string]::IsNullOrEmpty($_)) {$TempPath = $TempPath.Replace("$_;", "");}}}

# # Write-Host "TempPath" $TempPath -ForegroundColor Red
$TempPath = $TempPath + ';' + $PathVal + ';'

$Dupes = ($TempPath.Split(";") | Group-Object | Where-Object {$_.Count -gt 1}).Values;
if ($Dupes.Count -gt 0) {$Dupes | ForEach-Object {if (-not [string]::IsNullOrEmpty($_)) {$TempPath = $TempPath.Replace("$_;", ""); $TempPath += "$_;";}}}

$TempPath = $TempPath.Replace(";;", ";")

try
{
    [System.Environment]::SetEnvironmentVariable($VARIABLE, $TempPath, $SYSTEM_LEVEL)
}
catch
{
    if ($SYSTEM_LEVEL -eq 'machine')
    {
        Write-Host "Run as admin please" -ForegroundColor Red
    }
    else
    {
        Write-Host "unknown error when set env path"
    }
}
