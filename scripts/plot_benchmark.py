import json
import pandas as pd
import matplotlib.pyplot as plt
import os
import numpy as np

# --- Configuration ---

if len(sys.argv) < 3:
    print("Usage: python generate_graphs.py <input_json_path> <output_dir>")
    sys.exit(1)

INPUT_FILE = sys.argv[1]
OUTPUT_DIR = sys.argv[2] # This will be 'benchmark_graphs'



# Ensure the output directory exists
os.makedirs(OUTPUT_DIR, exist_ok=True)

# --- 1. Data Processing Function ---
def process_benchmark_data(file_path):
    """Loads benchmark JSON, calculates Average Time per Operation, and returns a DataFrame."""
    with open(file_path, 'r') as f:
        data = json.load(f)

    benchmarks = []

    for bm in data.get('benchmarks', []):
        name = bm['name']
        cpu_time_ns = bm['cpu_time']
        iterations = bm['iterations']

        # Extract the key features for grouping and display
        if 'SystemMalloc' in name:
            allocator = 'System Malloc'
        elif 'MyTLSFAllocatorFixture' in name:
            allocator = 'TLSF Allocator'
        else:
            continue # Skip non-relevant benchmarks

        # Determine the workload type ( Mixed)
        if 'AllocDeallocCycle_MixedSize' in name:
            workload = 'AllocDeallocCycle_MixedSize'
            # Extract N from the end of the name (e.g., '/100')
            try:
                num_ops = int(name.split('/')[-1])
            except ValueError:
                num_ops = 1 # Fallback
            # For mixed, CPU time is total time for N operations; Avg is total / N
            avg_time_ns = cpu_time_ns / num_ops
        else:
            continue

        benchmarks.append({
            'name': name,
            'allocator': allocator,
            'workload': workload,
            'num_ops': num_ops,
            'cpu_time_ns': cpu_time_ns,
            'avg_time_ns': avg_time_ns # Average time for a single allocation/operation
        })

    return pd.DataFrame(benchmarks)

# --- 2. Plotting Functions ---
def plot_performance(df_AllocDeallocCycle_MixedSize):
    """Generates a line plot showing My TLSF's performance vs. Malloc's"""

    # Sort for correct line plotting
    df_mixed_sorted = df_AllocDeallocCycle_MixedSize.sort_values(by='num_ops')

    plt.figure(figsize=(9, 6))

    # Plotting the lines
    for name, group in df_mixed_sorted.groupby('allocator'):
        plt.plot(group['num_ops'], group['avg_time_ns'],
                 marker='o', linestyle='-', label=name,
                 linewidth=2.5)

    plt.title('Performance Graph', fontsize=14)
    plt.ylabel('Average Time per N Allocation-Deallocation of Mixed Sizes (nanoseconds)', fontsize=12)
    plt.xlabel('N Allocations per iteration ', fontsize=12)
    plt.xticks(group['num_ops']) # Ensure clear ticks
    plt.legend(title='Allocator')
    plt.grid(linestyle='--', alpha=0.7)

    plt.savefig(os.path.join(OUTPUT_DIR, 'Performance_line_plot.svg'), format='svg', bbox_inches='tight')
    print(f"Generated {os.path.join(OUTPUT_DIR, 'Performance_line_plot.svg')}")
    plt.close()

# --- 3. Main Execution ---
if __name__ == '__main__':
    try:
        # Load and process the data
        df = process_benchmark_data(INPUT_FILE)
        print("Data loaded successfully.")
        print(f"Total benchmarks processed: {len(df)}")

        # Filter for the mixed workload (fragmentation stress)
        df_AllocDeallocCycle_MixedSize = df[df['workload'] == 'AllocDeallocCycle_MixedSize'].copy()

        if df_AllocDeallocCycle_MixedSize.empty:
            print("Error: Could not find 'AllocDeallocCycle_MixedSize' data. Check  JSON keys.")
        else:
            #time taken across various allocations
            plot_performance(df_AllocDeallocCycle_MixedSize)

        print("\nPlotting complete. SVGs saved to the 'output_graphs' directory.")

    except FileNotFoundError:
        print(f"Error: Input file '{INPUT_FILE}' not found. Place your benchmark JSON here.")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")
