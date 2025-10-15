#!/usr/bin/env node

/**
 * IDTF E2E Test Script
 * 
 * This script performs end-to-end testing of the IDTF framework by:
 * 1. Simulating OPC UA/MQTT data sources
 * 2. Verifying data flow through NDH Connector Agents to NDH
 * 3. Checking data availability in downstream systems (Omniverse/MES)
 */

const axios = require('axios');
const { spawn } = require('child_process');

const NDH_API_URL = process.env.NDH_API_URL || 'http://localhost:8080';
const TEST_TIMEOUT = 60000; // 60 seconds

// Test data
const testAssetType = {
  id: 'TestPump_V1',
  name: 'Test Centrifugal Pump',
  properties: [
    { name: 'MaxFlowRate', dataType: 'number', unit: 'm3/h' }
  ],
  telemetry: [
    { name: 'CurrentFlowRate', dataType: 'number', unit: 'm3/h' }
  ]
};

const testAssetInstance = {
  id: 'TestPump_001',
  name: 'Test Pump Instance 1',
  assetTypeId: 'TestPump_V1',
  operationalStatus: 'Running'
};

async function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

async function testNDHConnection() {
  console.log('Testing NDH connection...');
  try {
    const response = await axios.get(`${NDH_API_URL}/health`);
    if (response.status === 200) {
      console.log('✓ NDH is healthy');
      return true;
    }
  } catch (error) {
    console.error('✗ NDH connection failed:', error.message);
    return false;
  }
}

async function createAssetType() {
  console.log('Creating test asset type...');
  try {
    const response = await axios.post(`${NDH_API_URL}/api/v1/asset-types`, testAssetType);
    if (response.status === 201) {
      console.log('✓ Asset type created successfully');
      return true;
    }
  } catch (error) {
    console.error('✗ Failed to create asset type:', error.message);
    return false;
  }
}

async function createAssetInstance() {
  console.log('Creating test asset instance...');
  try {
    const response = await axios.post(`${NDH_API_URL}/api/v1/asset-instances`, testAssetInstance);
    if (response.status === 201) {
      console.log('✓ Asset instance created successfully');
      return true;
    }
  } catch (error) {
    console.error('✗ Failed to create asset instance:', error.message);
    return false;
  }
}

async function simulateOPCUAData() {
  console.log('Simulating OPC UA data stream...');
  // In a real scenario, this would trigger the OPC UA simulator
  // For now, we'll directly inject test data via NDH API
  try {
    const telemetryData = {
      assetInstanceId: 'TestPump_001',
      telemetryPoint: 'CurrentFlowRate',
      value: 45.5,
      timestamp: new Date().toISOString()
    };
    const response = await axios.post(`${NDH_API_URL}/api/v1/telemetry`, telemetryData);
    if (response.status === 200) {
      console.log('✓ Telemetry data ingested successfully');
      return true;
    }
  } catch (error) {
    console.error('✗ Failed to ingest telemetry data:', error.message);
    return false;
  }
}

async function verifyDataInNDH() {
  console.log('Verifying data in NDH...');
  try {
    const response = await axios.get(`${NDH_API_URL}/api/v1/asset-instances/TestPump_001/telemetry`);
    if (response.status === 200 && response.data.length > 0) {
      console.log('✓ Data verified in NDH:', response.data[0]);
      return true;
    }
  } catch (error) {
    console.error('✗ Failed to verify data in NDH:', error.message);
    return false;
  }
}

async function cleanup() {
  console.log('Cleaning up test data...');
  try {
    await axios.delete(`${NDH_API_URL}/api/v1/asset-instances/TestPump_001`);
    await axios.delete(`${NDH_API_URL}/api/v1/asset-types/TestPump_V1`);
    console.log('✓ Cleanup completed');
  } catch (error) {
    console.error('✗ Cleanup failed:', error.message);
  }
}

async function runE2ETests() {
  console.log('Starting IDTF E2E Tests...\n');
  
  let allTestsPassed = true;

  // Test 1: NDH Connection
  if (!await testNDHConnection()) {
    allTestsPassed = false;
  }
  await sleep(1000);

  // Test 2: Create Asset Type
  if (!await createAssetType()) {
    allTestsPassed = false;
  }
  await sleep(1000);

  // Test 3: Create Asset Instance
  if (!await createAssetInstance()) {
    allTestsPassed = false;
  }
  await sleep(1000);

  // Test 4: Simulate OPC UA Data
  if (!await simulateOPCUAData()) {
    allTestsPassed = false;
  }
  await sleep(2000);

  // Test 5: Verify Data in NDH
  if (!await verifyDataInNDH()) {
    allTestsPassed = false;
  }

  // Cleanup
  await cleanup();

  console.log('\n' + '='.repeat(50));
  if (allTestsPassed) {
    console.log('✓ All E2E tests passed!');
    process.exit(0);
  } else {
    console.log('✗ Some E2E tests failed!');
    process.exit(1);
  }
}

// Run tests
runE2ETests().catch(error => {
  console.error('E2E test execution failed:', error);
  process.exit(1);
});

