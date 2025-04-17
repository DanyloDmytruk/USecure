# PE Yara rules checker
# Rules repository: https://github.com/Yara-Rules/rules/tree/master

import yara
import os
import sys
import glob

def load_yara_rules_from_dirs(directories):
    rule_files = []
    for dir_path in directories:
        rule_files.extend(glob.glob(os.path.join(dir_path, '*.yar')) + glob.glob(os.path.join(dir_path, '*.yara')))

    rules_dict = {}
    for idx, rule_file in enumerate(rule_files):
        try:
            rules_dict[f"rule_{idx}"] = rule_file
        except Exception as e:
            print(f"Failed to include {rule_file}: {e}")
    return yara.compile(filepaths=rules_dict)

def main():
    if len(sys.argv) != 2:
        print("Usage: python scan.py path_to_exe")
        sys.exit(1)

    exe_path = sys.argv[1]
    if not os.path.isfile(exe_path):
        print(f"File not found: {exe_path}")
        sys.exit(1)

    rule_dirs = [
        "rules/antidebug_antivm",
        "rules/cve_rules",
        "rules/malware",
        "rules/packers"
    ]

    try:
        try:
            rules = load_yara_rules_from_dirs(rule_dirs)
        except Exception as e:
            print(f"[ERROR LOAD] Can not load yara rules {e}")
            return

        matches = rules.match(exe_path)
        if matches:
            for match in matches:
                print(f"[RESULT] Detected. Yara rule: {match.rule}")
        else:
            print("No detection.")
    except yara.Error as e:
        print(f"YARA error: {e}")

if __name__ == "__main__":
    main()


