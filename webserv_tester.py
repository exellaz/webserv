import csv
import subprocess
import shlex

# ANSI colors
RED     = "\033[31m"
GREEN   = "\033[32m"
YELLOW  = "\033[33m"
BLUE    = "\033[34m"
MAGENTA = "\033[35m"
CYAN    = "\033[36m"
RESET   = "\033[0m"
BOLD    = "\033[1m"

base_url = "http://127.0.0.1:4243"

status_reason = {
    "200": "OK",
    "201": "Created",
    "204": "No Content",
    "301": "Moved Permanently",
    "302": "Found",
    "400": "Bad Request",
    "401": "Unauthorized",
    "403": "Forbidden",
    "404": "Not Found",
    "405": "Method Not Allowed",
    "413": "Payload Too Large",
    "500": "Internal Server Error",
    "501": "Not Implemented",
    "504": "Gateway Timeout",
    "505": "HTTP Version Not Supported"
}

def color(text, code):
    return f"{code}{text}{RESET}"

def status_phrase(code):
    return status_reason.get(code, "Unknown")

def run_curl(url, method, extra_args):
    cmd = ["curl", "-X", method] + shlex.split(extra_args) + [
        "-s", "-D", "-", "-o", "/dev/null", "-w", "%{http_code}", url
    ]
    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=10)
        status_code = result.stdout.strip()[-3:]
        return status_code
    except subprocess.TimeoutExpired:
        return "timeout"
    except Exception as e:
        return f"error: {str(e)}"

def parse_test_csv(filename):
    test_cases = []
    with open(filename, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            method   = row["method"].strip()
            endpoint = row["endpoint"].strip()
            options  = row["options"].strip()
            expected = row["expected"].strip()

            # Interpret 'NONE' (case-insensitive) as empty options
            if options.upper() == "NONE":
                options = ""

            test_cases.append((method, endpoint, options, expected))
    return test_cases

def main():
    print(BOLD + "\n=== Webserv Tester ===\n" + RESET)
    test_cases = parse_test_csv("tests.csv")

    for method, endpoint, curl_opts, expected_status in test_cases:
        url = base_url + endpoint
        actual_status = run_curl(url, method, curl_opts)

        passed = (
            (expected_status == "2xx" and actual_status.startswith("2")) or
            (actual_status == expected_status)
        )

        result = color("[✓]", GREEN) if passed else color("[✗]", RED)

        reason = status_phrase(actual_status)
        status_info = f"{actual_status} {reason}"

        if actual_status == "timeout":
            status_info = color("timeout", RED)
            expected_color = RED
        elif actual_status.startswith("2"):
            status_info = color(status_info, GREEN)
            expected_color = GREEN
        elif actual_status.startswith("4"):
            status_info = color(status_info, RED)
            expected_color = RED
        elif actual_status.startswith("5"):
            status_info = color(status_info, YELLOW)
            expected_color = YELLOW
        else:
            status_info = color(status_info, MAGENTA)
            expected_color = MAGENTA

        print(f"{result} {method:<6} {endpoint:<25} -> "
              f"Status: {status_info:<32} | "
              f"Expected: {color(expected_status, expected_color)}")

if __name__ == "__main__":
    main()
