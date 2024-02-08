import subprocess
import sys
def run_test(output_folder, module_path):
    # Run the testing script to generate output files
    subprocess.run(['./runit.sh', output_folder, module_path])


def score_blackbox(blackbox_folder, module_folder):
    # Run the scoring script to compare results
    result = subprocess.run(['./gradeit.sh', blackbox_folder, module_folder], capture_output=True)
    return result.stdout.decode().strip()


if __name__ == "__main__":
    input_folder = "./geninputcycle"
    answer_folder = "./genanswercycle"
    out_folder = "./genoutcycle"
    my_path = "./mycode/linker"
    answer_path = "./linker"
    count = 1
    while True:
        # Generate input data
        subprocess.run([sys.executable,'./lab1gen.py'])  # folder hard coded to geninputcycle 20 files

        # Run testing script
        run_test(answer_folder, answer_path)
        run_test(out_folder, my_path)

        # Score blackbox vs module
        result = score_blackbox(answer_folder, out_folder)
        print("The " + str(count) + "round")
        print(result)
        count += 1
        # Check if any test failed
        if '0' in result.split()[4:]:
            print("Test failed. Stopping...")
            break
