"""
Simple NDH Push/Pull Example
Demonstrates basic data push and pull operations
"""

import asyncio
import requests
from datetime import datetime


# NDH server URL
NDH_URL = "http://localhost:8000"


def push_data_sync():
    """Push data to NDH using synchronous requests"""
    print("=" * 60)
    print("Pushing data to NDH...")
    print("=" * 60)
    
    # Prepare data
    data = {
        "asset_id": "PUMP_001",
        "timestamp": datetime.now().isoformat(),
        "values": {
            "temperature": 75.5,
            "pressure": 120.0,
            "flow_rate": 50.2,
            "status": "running"
        },
        "quality": "good",
        "source": "example_script"
    }
    
    # Push data
    response = requests.post(f"{NDH_URL}/api/v1/data/push", json=data)
    
    if response.status_code == 200:
        result = response.json()
        print(f"✓ Data pushed successfully!")
        print(f"  Data ID: {result.get('data_id')}")
        print(f"  Status: {result.get('status')}")
    else:
        print(f"✗ Failed to push data: {response.status_code}")
        print(f"  Error: {response.text}")


def push_batch_data_sync():
    """Push multiple data points to NDH"""
    print("\n" + "=" * 60)
    print("Pushing batch data to NDH...")
    print("=" * 60)
    
    # Prepare batch data
    data_points = [
        {
            "asset_id": f"SENSOR_{i:03d}",
            "timestamp": datetime.now().isoformat(),
            "values": {
                "temperature": 20.0 + i * 0.5,
                "humidity": 50.0 + i * 0.3
            },
            "quality": "good"
        }
        for i in range(10)
    ]
    
    # Push batch
    response = requests.post(f"{NDH_URL}/api/v1/data/push/batch", json=data_points)
    
    if response.status_code == 200:
        result = response.json()
        print(f"✓ Batch data pushed successfully!")
        print(f"  Message: {result.get('message')}")
        print(f"  Status: {result.get('status')}")
    else:
        print(f"✗ Failed to push batch data: {response.status_code}")


def check_health():
    """Check NDH service health"""
    print("\n" + "=" * 60)
    print("Checking NDH service health...")
    print("=" * 60)
    
    response = requests.get(f"{NDH_URL}/health")
    
    if response.status_code == 200:
        health = response.json()
        print(f"✓ NDH service is healthy!")
        print(f"  Status: {health.get('status')}")
        print(f"  Version: {health.get('version')}")
        print(f"  Environment: {health.get('environment')}")
    else:
        print(f"✗ NDH service is not responding")


def get_config():
    """Get NDH configuration"""
    print("\n" + "=" * 60)
    print("Getting NDH configuration...")
    print("=" * 60)
    
    response = requests.get(f"{NDH_URL}/api/v1/config")
    
    if response.status_code == 200:
        config = response.json()
        print(f"✓ Configuration retrieved!")
        print(f"  Server: {config['config']['server']}")
        print(f"  Redis: {config['config']['redis']}")
    else:
        print(f"✗ Failed to get configuration")


def main():
    """Main function"""
    print("\n" + "=" * 60)
    print("NDH Simple Push/Pull Example")
    print("=" * 60)
    
    try:
        # Check health first
        check_health()
        
        # Get configuration
        get_config()
        
        # Push single data point
        push_data_sync()
        
        # Push batch data
        push_batch_data_sync()
        
        print("\n" + "=" * 60)
        print("Example completed successfully!")
        print("=" * 60)
    
    except requests.exceptions.ConnectionError:
        print("\n✗ Error: Cannot connect to NDH service")
        print("  Please make sure NDH is running at http://localhost:8000")
        print("  Start NDH with: python -m ndh.main")
    
    except Exception as e:
        print(f"\n✗ Error: {e}")


if __name__ == "__main__":
    main()

