max_show = 10


def compare_files(file1_path, file2_path):
    # Open the files
    with open(file1_path, "r", encoding="utf-8") as f1, open(
        file2_path, "r", encoding="utf-8"
    ) as f2:
        count = 0
        # Read lines one by one
        for line_number, (line1, line2) in enumerate(zip(f1, f2), 1):
            # Compare the two lines
            if line1 == line2:
                continue

            count = count + 1
            if count > max_show:
                print("there are more lines...")
                return

            # when line1 != line2
            print(f"Line {line_number} does not match.")
            print(f"> {line1.strip()}")
            print(f"> {line2.strip()}\n")


if __name__ == "__main__":
    import sys

    # Get command-line arguments
    if len(sys.argv) != 3:
        print("Usage: compare.py <file1_path> <file2_path>")
        sys.exit(1)

    file1_path = sys.argv[1]
    file2_path = sys.argv[2]

    # Perform file comparison
    compare_files(file1_path, file2_path)
