$ErrorActionPreference = 'Stop'
Push-Location $PSScriptRoot
try {
    New-Item -ItemType Directory -Force build/classes, build/test-classes | Out-Null
    $mainSources = @(Get-ChildItem src/main/java -Recurse -Filter *.java | ForEach-Object FullName)
    & javac --release 17 -encoding UTF-8 -d build/classes $mainSources
    if ($LASTEXITCODE -ne 0) { throw 'Main compilation failed.' }
    $testSources = @(Get-ChildItem src/test/java -Recurse -Filter *.java | ForEach-Object FullName)
    & javac --release 17 -encoding UTF-8 -cp build/classes -d build/test-classes $testSources
    if ($LASTEXITCODE -ne 0) { throw 'Test compilation failed.' }
    Write-Host 'Build succeeded (Java 17 target).'
} finally { Pop-Location }
