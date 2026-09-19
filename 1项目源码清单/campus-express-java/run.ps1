$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    & ./build.ps1
    & java -cp build/classes campus.express.Main
    if ($LASTEXITCODE -ne 0) { throw 'Application failed.' }
} finally { Pop-Location }
