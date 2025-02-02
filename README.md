# CS6135 Homework 3: Fixed-outline Floorplan Design

## 📌 Introduction
This project implements **Simulated Annealing (SA)** for **Fixed-outline Floorplan Design** using a set of **hard blocks**.  
The goal is to **minimize wirelength and optimize floorplan layout** while satisfying the **fixed-outline constraint**.

## 📂 Project Structure
```
CS6135_HW3_{STUDENT_ID}/
│── src/                     # Source code and Makefile
│   ├── main.cpp             # Main function (SA-based floorplanning)
│   ├── SA.cpp               # Simulated Annealing algorithm implementation
│   ├── SA.hpp               # SA class definitions
│   ├── Makefile             # Compilation script
│── bin/                     # Stores compiled executable (hw3)
│── output/                  # Stores floorplan output files
│── testcases/               # Testcases for evaluation
│── run.sh                   # Automated execution script
│── CS6135_HW3_{STUDENT_ID}_report.pdf  # Report file
│── README.md                # This document
```

## ⚙️ Installation & Compilation

### **Step 1: Compile the program**
Navigate to the `src/` directory and compile using `make`:
```bash
cd src
make
```
This will generate the executable **`hw3`** inside the `bin/` directory.

### **Step 2: Clean compiled files (optional)**
To remove compiled binaries, use:
```bash
make clean
```

## 🚀 Execution

### **Option 1: Run manually**
You can manually run the program using the following command:
```bash
./bin/hw3 <hardblocks_file> <nets_file> <pl_file> <output_floorplan> <dead_space_ratio>
```
Example:
```bash
./bin/hw3 testcases/n100.hardblocks testcases/n100.nets testcases/n100.pl output/n100_0.15.floorplan 0.15
```

### **Option 2: Run using script (`run.sh`)**
To automate execution for different testcases, use:
```bash
bash run.sh <testcase_number>
```
where `<testcase_number>` can be:
- `1` → Run `n100` testcase
- `2` → Run `n200` testcase
- `3` → Run `n300` testcase

Example:
```bash
bash run.sh 1
```

### **Arguments:**
- `<hardblocks_file>`: Specifies the hard block definitions.
- `<nets_file>`: Specifies the netlist.
- `<pl_file>`: Specifies pin locations.
- `<output_floorplan>`: Stores the generated floorplan result.
- `<dead_space_ratio>`: Defines the allowed dead space ratio (e.g., 0.1 or 0.15).

## 📊 Expected Output

The program generates a `.floorplan` file containing:
- **Wirelength computation** (using HPWL - Half Perimeter Wirelength)
- **Final block placements** (X, Y, rotation)

Example `.floorplan` output:
```
Wirelength 75563
Blocks
sb0 152 284 1
sb1 126 179 0
...
```

## 📄 Report Requirements

Your submission must include:
1. **Your name and student ID**.
2. **Compilation and execution instructions**, with examples.
3. **Results for different testcases** (`n100`, `n200`, `n300`) using `dead_space_ratio = 0.1` and `0.15`.
4. **Minimum feasible dead space ratio** within **20-minute execution time**.
5. **Algorithm explanation** (flowchart and/or pseudocode).
6. **Optimization techniques** used to improve performance.
7. **Comparison with last year's top 5 results** (`dead_space_ratio = 0.15`).
8. **Challenges faced & lessons learned**.

## 🔥 Submission Guidelines
Before submission, compress your directory:
```bash
tar -zcvf CS6135_HW3_{STUDENT_ID}.tar.gz CS6135_HW3_{STUDENT_ID}/
```
Upload `CS6135_HW3_{STUDENT_ID}.tar.gz` to **iLMS**.

---
### **⚠️ Important Notes**
- Ensure `make` compiles successfully.
- Do **NOT** hardcode paths in your code.
- Execution time **must be within 20 minutes per testcase**.
- Your program will be tested using `HW3_grading.sh`.

---

## 📌 Author
**Student Name**: Yi-Shan Lan
