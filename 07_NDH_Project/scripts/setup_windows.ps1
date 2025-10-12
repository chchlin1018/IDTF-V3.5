# NDH Test Framework - Windows Setup Script
# This script helps set up the NDH environment on Windows

Write-Host "=" * 60
Write-Host "NDH Test Framework - Windows Setup"
Write-Host "=" * 60

# Check Python installation
Write-Host "`nChecking Python installation..."
try {
    $pythonVersion = python --version 2>&1
    Write-Host "✓ Python found: $pythonVersion"
    
    # Check if Python 3.11+
    if ($pythonVersion -match "Python 3\.(\d+)") {
        $minorVersion = [int]$matches[1]
        if ($minorVersion -lt 11) {
            Write-Host "✗ Python 3.11 or higher is required"
            Write-Host "  Please download from: https://www.python.org/downloads/"
            exit 1
        }
    }
} catch {
    Write-Host "✗ Python not found"
    Write-Host "  Please install Python 3.11+ from: https://www.python.org/downloads/"
    exit 1
}

# Create virtual environment
Write-Host "`nCreating virtual environment..."
if (Test-Path "venv") {
    Write-Host "  Virtual environment already exists"
} else {
    python -m venv venv
    Write-Host "✓ Virtual environment created"
}

# Activate virtual environment
Write-Host "`nActivating virtual environment..."
& .\venv\Scripts\Activate.ps1

# Upgrade pip
Write-Host "`nUpgrading pip..."
python -m pip install --upgrade pip

# Install dependencies
Write-Host "`nInstalling dependencies..."
pip install -r requirements.txt

Write-Host "`n" + "=" * 60
Write-Host "Setup completed successfully!"
Write-Host "=" * 60

Write-Host "`nNext steps:"
Write-Host "1. Activate the virtual environment:"
Write-Host "   .\venv\Scripts\Activate.ps1"
Write-Host ""
Write-Host "2. Start the NDH service:"
Write-Host "   python -m ndh.main"
Write-Host ""
Write-Host "3. In another terminal, run the example:"
Write-Host "   python examples\basic\simple_push_pull.py"
Write-Host ""
Write-Host "4. Access the API documentation:"
Write-Host "   http://localhost:8000/docs"
Write-Host ""

# Optional: Check Redis
Write-Host "`nOptional: Redis Setup"
Write-Host "Redis is optional but recommended for production use."
Write-Host "To install Redis on Windows:"
Write-Host "  Option 1: Use WSL2 (Recommended)"
Write-Host "    wsl --install"
Write-Host "    wsl"
Write-Host "    sudo apt-get install redis-server"
Write-Host ""
Write-Host "  Option 2: Use Memurai (Redis for Windows)"
Write-Host "    Download from: https://www.memurai.com/"
Write-Host ""

