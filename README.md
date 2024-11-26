# VT-tree

## How to Compile ⚙️

Run the `make` command in each directory to generate the executable file:

```bash
make
```

## How to Run 🚀

Execute the `test.sh` script in each directory to run the program and start testing:

```bash
./test.sh
```

## How to Change the Dataset 🔄

Modify the file path for `jsonHelper` in `main.cpp`.

For example:  
```cpp
jsonHelper.readSingleJson("./data/shibuya/json/0-baseline.json");
```

You can choose a different dataset from the following options:

- beach
- shibuya
- warsaw

## Additional Notes 📑
- **Dataset Location**: The datasets are located in the data folder under the relevant subdirectory.
- **Test Data**: The test folder contains various text files with pre-generated test cases.