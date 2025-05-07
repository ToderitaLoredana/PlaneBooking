import subprocess

   
def run():
    exe_path = "./main.exe"
    input_file = "data.json"
    output_file = "output.json"
    source = "NYC"
    destination = "BER"
    day = "Monday"
    departure_time = "08:00"  # Optional; include if needed

# Build the argument list
    args = [
        exe_path,
        input_file,
        output_file,
        source,
        destination,
        day,
        departure_time  # Remove this line if you don't want to include it
    ]
    try:
    # Execute the command
        subprocess.run(args, check=True)
        print("Execution successful.")
    except subprocess.CalledProcessError as e:
        print("Execution failed:", e)
    except FileNotFoundError:
        print("Executable not found. Check the path.")
        
        
run()