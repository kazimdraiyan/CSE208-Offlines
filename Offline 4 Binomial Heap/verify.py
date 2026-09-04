# Check the output of the generated files against the expected output files.

from pathlib import Path

BASE_DIR = Path(__file__).parent

for i in range(1, 11):
    expected = BASE_DIR / f"test_{i:02d}" / "output.txt"
    generated = BASE_DIR / f"output_{i:02d}.txt"

    if not expected.exists():
        print(f"Test {i:02d}: expected file not found")
        continue

    if not generated.exists():
        print(f"Test {i:02d}: generated file not found")
        continue

    expected_text = expected.read_text(encoding="utf-8").replace("\r\n", "\n")
    generated_text = generated.read_text(encoding="utf-8").replace("\r\n", "\n")

    if expected_text == generated_text:
        print(f"Test {i:02d}: PASS")
    else:
        print(f"Test {i:02d}: FAIL")
