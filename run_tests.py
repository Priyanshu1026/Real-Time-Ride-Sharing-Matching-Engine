import subprocess
import re
import os

# Configuration
EXECUTABLE = "./build/Debug/ride_matching_simulator.exe" # Windows
# EXECUTABLE = "./build/ride_matching_simulator"   # Linux/Mac (Uncomment if needed)

TEST_CASES = [
    {
        "file": "samples/test_basic.csv",
        "name": "Basic Match",
        "expected_rate": 100.0
    },
    {
        "file": "samples/test_retry.csv",
        "name": "Retry Logic",
        "expected_rate": 100.0
    },
    {
        "file": "samples/test_fail.csv",
        "name": "Impossible Match",
        "expected_rate": 0.0
    }
]

def run_test(test):
    print(f"🔹 Running Test: {test['name']} ({test['file']})...")
    
    # Check if file exists
    if not os.path.exists(test['file']):
        print(f"❌ Error: File {test['file']} not found.")
        return False

    # Run the C++ Simulator
    try:
        result = subprocess.run([EXECUTABLE, test['file']], capture_output=True, text=True)
        output = result.stdout
    except FileNotFoundError:
        print(f"❌ Error: Executable {EXECUTABLE} not found. Did you build?")
        return False

    # Find Match Rate in output using Regex
    match = re.search(r"Match Rate\s+:\s+(\d+\.\d+)%", output)
    
    if match:
        actual_rate = float(match.group(1))
        if actual_rate == test['expected_rate']:
            print(f"✅ PASS. (Expected {test['expected_rate']}%, Got {actual_rate}%)")
            return True
        else:
            print(f"❌ FAIL. (Expected {test['expected_rate']}%, Got {actual_rate}%)")
            # Optional: Print output for debugging
            # print(output)
            return False
    else:
        print("❌ FAIL. Could not find Match Rate in output.")
        return False

# Main Loop
print("========================================")
print("      AUTOMATED INTEGRATION TESTS       ")
print("========================================")

passed = 0
for test in TEST_CASES:
    if run_test(test):
        passed += 1
    print("-" * 40)

print(f"Summary: {passed}/{len(TEST_CASES)} Tests Passed.")
if passed == len(TEST_CASES):
    print("🎉 ALL SYSTEMS GO!")
    exit(0)
else:
    print("⚠️ SOME TESTS FAILED.")
    exit(1)