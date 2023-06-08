import numpy as np

# Generate a 100x100 matrix
matrix = np.random.randint(0, 10, size=(3, 3))

# Generate a single row with 100 columns
single_row = np.random.randint(0, 10, size=(1, 3))

# Save the matrix and single row to a text file
with open('matrix.txt', 'w') as file:
    # Write the size of the matrix
    file.write(f'{matrix.shape[0]}\n')

    # Save the matrix to the file
    np.savetxt(file, matrix, fmt='%d')
    
    # Write the size of the matrix
    file.write(f'{matrix.shape[0]}\n')

    # Save the single row to the file
    np.savetxt(file, single_row, fmt='%d')
