import os

def readFiles(directory):
    times = []
    for filename in os.listdir(directory):
        if filename.endswith(".csv"):
            with open(os.path.join(directory, filename), 'r') as file:
                fTimes = [float(line.strip()) for line in file]
                times.extend(fTimes)
    return times

def average(times):
    return sum(times) / len(times)

def speedup(sequentialTime, parallelTime):
    return sequentialTime / parallelTime

if __name__ == "__main__":
    sequentialDir = './logs'
    parallelDir = './plogs'

    sequentialTimes = readFiles(sequentialDir)
    parallelTimes = readFiles(parallelDir)

    sequentialAvg = average(sequentialTimes)
    parallelAvg = average(parallelTimes)

    speedup = speedup(sequentialAvg, parallelAvg)
    efficiency = speedup / 4

    print(f"Tiempo promedio secuencial: {sequentialAvg:.6f}s")
    print(f"Tiempo promedio paralelo: {parallelAvg:.6f}s")
    print(f"Speedup: {speedup:.2f}")
    print(f"Efficiency: {efficiency:.2f}")
