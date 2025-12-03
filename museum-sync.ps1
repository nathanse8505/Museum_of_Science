param(
    [string]$CommitMessage
)

# Always work from the folder where this script lives
$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $scriptDir

if (-not $CommitMessage) {
    $CommitMessage = Read-Host "Enter commit message for all repos"
}

$committed = @()
$pushed    = @()
Get-ChildItem -Directory | ForEach-Object {
    $repoPath = $_.FullName
    $gitDir   = Join-Path $repoPath ".git"

    if (-not (Test-Path $gitDir)) {
        return
    }

    Push-Location $repoPath

    # Check if there is at least one commit
    $hasCommit = $true
    git rev-parse --verify HEAD *> $null
    if ($LASTEXITCODE -ne 0) {
        $hasCommit = $false
    }

    $needsCommit = $false
    $needsPush   = $false

    if (-not $hasCommit) {
        # No commits yet – any files means we need a commit (and later a push)
        $status = git status --porcelain
        if ($status) {
            $needsCommit = $true
        }
    }
    else {
        # Repo has commits – check diff vs HEAD (uncommitted changes)
        git diff --quiet HEAD --
        if ($LASTEXITCODE -ne 0) {
            $needsCommit = $true
        }

        # Check if branch is ahead of remote (unpushed commits)
        $statusOut = git status -sb
        if ($statusOut -match "ahead") {
            $needsPush = $true
        }
    }

    if ($needsCommit -or $needsPush) {
        Write-Host "===================================================="
        Write-Host "Repo: $($_.Name)"
        Write-Host "===================================================="
        git status -sb

        if ($needsCommit) {
            Write-Host "Adding and committing..."
            git add -A
            git commit -m "$CommitMessage" *> $null

            if ($LASTEXITCODE -ne 0) {
                Write-Host "Commit failed (nothing to commit or error)."
            } else {
                $committed += $_.Name
            }
        } else {
            Write-Host "No new changes to commit, but branch is ahead of remote. Will push."
        }

        # Push if we committed something or branch was already ahead
        if ($needsCommit -or $needsPush) {
            Write-Host "Pushing..."
            git push --set-upstream origin HEAD
            if ($LASTEXITCODE -ne 0) {
                Write-Host "Push failed for $($_.Name) (no remote/upstream or other error)."
            } else {
                $pushed += $_.Name
            }
        }

        Write-Host
    }

    Pop-Location
}

Write-Host "===================================================="
Write-Host "Summary"
Write-Host "===================================================="

if ($committed.Count -gt 0) {
    Write-Host "Committed in repos:"
    "  " + ($committed -join ", ")
} else {
    Write-Host "No repos had changes to commit."
}

if ($pushed.Count -gt 0) {
    Write-Host "Pushed successfully for:"
    "  " + ($pushed -join ", ")
} else {
    Write-Host "No pushes were performed."
}

Write-Host
Read-Host "Press Enter to exit"
