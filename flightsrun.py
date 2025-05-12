# flightsrun.py
import subprocess

def run(source, destination, day, departure_time):
    exe_path = "./main.exe"
    input_file = "data.json"
    output_file = "output.json"

    # Build the argument list
    args = [
        exe_path,
        input_file,
        output_file,
        source,
        destination,
        day,
        departure_time
    ]

    try:
        # Execute the command
        subprocess.run(args, check=True)
        print("Execution successful.")
    except subprocess.CalledProcessError as e:
        print("Execution failed:", e)
    except FileNotFoundError:
        print("Executable not found. Check the path.")
