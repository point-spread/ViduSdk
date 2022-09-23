$PathVal = $args[0]
$Level = $args[1]

$SYSTEM_LEVEL = 'user'
if ($Level -eq 'true'){
    $SYSTEM_LEVEL = 'machine'
}
 
$orig = [System.Environment]::GetEnvironmentVariable('PATH', $SYSTEM_LEVEL)

try{
[System.Environment]::SetEnvironmentVariable('PATH',$orig+';'+$PathVal, $SYSTEM_LEVEL)
}catch{
    if($SYSTEM_LEVEL -eq 'machine'){
        Write-Host "Run as admin please" -ForegroundColor Red
    }else{
        Write-Host "unknown error when set env path"
    }
}
