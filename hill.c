#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Function to calculate the determinant of a 3x3 matrix
int determinant(int key[3][3]) {
    return key[0][0] * (key[1][1] * key[2][2] - key[1][2] * key[2][1]) -
           key[0][1] * (key[1][0] * key[2][2] - key[1][2] * key[2][0]) +
           key[0][2] * (key[1][0] * key[2][1] - key[1][1] * key[2][0]);
}

// Function to calculate the modular inverse of a number (a) under modulo m
int modInverse(int a, int m) {
    a = a % m;
    for (int x = 1; x < m; x++) {
        if ((a * x) % m == 1) {
            return x;
        }
    }
    return -1;  // No modular inverse exists
}

// Function to multiply two matrices and take the result modulo 26
void matrixMultiply(int key[3][3], int text[3], int result[3], int mod) {
    for (int i = 0; i < 3; i++) {
        result[i] = 0;
        for (int j = 0; j < 3; j++) {
            result[i] += key[i][j] * text[j];
        }
        result[i] %= mod;
    }
}

// Function to compute the inverse of a 3x3 matrix
void matrixInverse(int key[3][3], int inverseKey[3][3]) {
    int det = determinant(key);
    int detInv = modInverse(det, 26);

    if (detInv == -1) {
        printf("Inverse of the determinant doesn't exist. Inversion is not possible.\n");
        return;
    }

    inverseKey[0][0] = (detInv * (key[1][1] * key[2][2] - key[1][2] * key[2][1])) % 26;
    inverseKey[0][1] = (-detInv * (key[0][1] * key[2][2] - key[0][2] * key[2][1])) % 26;
    inverseKey[0][2] = (detInv * (key[0][1] * key[1][2] - key[0][2] * key[1][1])) % 26;
    inverseKey[1][0] = (-detInv * (key[1][0] * key[2][2] - key[1][2] * key[2][0])) % 26;
    inverseKey[1][1] = (detInv * (key[0][0] * key[2][2] - key[0][2] * key[2][0])) % 26;
    inverseKey[1][2] = (-detInv * (key[0][0] * key[1][2] - key[0][2] * key[1][0])) % 26;
    inverseKey[2][0] = (detInv * (key[1][0] * key[2][1] - key[1][1] * key[2][0])) % 26;
    inverseKey[2][1] = (-detInv * (key[0][0] * key[2][1] - key[0][1] * key[2][0])) % 26;
    inverseKey[2][2] = (detInv * (key[0][0] * key[1][1] - key[0][1] * key[1][0])) % 26;

    // Adjust negative values to be within range [0, 25]
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (inverseKey[i][j] < 0) {
                inverseKey[i][j] += 26;
            }
        }
    }
}

// Function to encrypt the plaintext using the Hill cipher
void encrypt(int key[3][3], char plaintext[3], char ciphertext[3]) {
    int textVector[3], result[3];
    for (int i = 0; i < 3; i++) {
        textVector[i] = plaintext[i] - 'A';
    }

    matrixMultiply(key, textVector, result, 26);

    for (int i = 0; i < 3; i++) {
        ciphertext[i] = (result[i] + 26) % 26 + 'A';  // Ensuring positive values
    }
}

// Function to decrypt the ciphertext using the Hill cipher
void decrypt(int key[3][3], char ciphertext[3], char plaintext[3]) {
    int inverseKey[3][3];
    matrixInverse(key, inverseKey);

    int textVector[3], result[3];
    for (int i = 0; i < 3; i++) {
        textVector[i] = ciphertext[i] - 'A';
    }

    matrixMultiply(inverseKey, textVector, result, 26);

    for (int i = 0; i < 3; i++) {
        plaintext[i] = (result[i] + 26) % 26 + 'A';  // Ensuring positive values
    }
}

// Function to replace spaces with a special character (e.g., 'X')
void replaceSpaces(char* str, char specialChar) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ' ') {
            str[i] = specialChar;
        }
    }
}

// Function to restore spaces after decryption
void restoreSpaces(char* str, char specialChar) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == specialChar) {
            str[i] = ' ';
        }
    }
}

int main() {
    int key[3][3];
    char plaintext[100];
    char ciphertext[100];
    char decryptedtext[100];
    char specialChar = 'X';  // Use 'X' as the replacement for spaces

    // Input the key matrix
    printf("Enter the 3x3 key matrix (values between 0 and 25):\n");
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("key[%d][%d]: ", i, j);
            scanf("%d", &key[i][j]);
        }
    }

    // Calculate the determinant and check for invertibility
    int det = determinant(key);
    int detInv = modInverse(det, 26);

    printf("Determinant: %d\n", det);
    printf("Determinant Inverse: %d\n", detInv);

    if (detInv == -1) {
        printf("The key matrix is not invertible (no modular inverse for determinant modulo 26).\n");
        return 1;
    }

    // Prompt the user to input the plaintext
    printf("Enter a plaintext sentence (A-Z, spaces allowed): ");
    getchar();  // Clear newline character from input buffer
    fgets(plaintext, 100, stdin);

    // Remove newline character if present
    size_t len = strlen(plaintext);
    if (plaintext[len - 1] == '\n') {
        plaintext[len - 1] = '\0';
    }

    // Convert to uppercase and replace spaces with a special character
    for (int i = 0; plaintext[i] != '\0'; i++) {
        plaintext[i] = toupper(plaintext[i]);
    }
    replaceSpaces(plaintext, specialChar);

    // Pad with 'X' if the length is not a multiple of 3
    size_t textLength = strlen(plaintext);
    while (textLength % 3 != 0) {
        plaintext[textLength++] = 'X';
        plaintext[textLength] = '\0';
    }

    // Encrypt the plaintext in blocks of 3 letters
    for (size_t i = 0; i < textLength; i += 3) {
        encrypt(key, &plaintext[i], &ciphertext[i]);
    }
    ciphertext[textLength] = '\0';  // Null-terminate the ciphertext string

    printf("Ciphertext: %s\n", ciphertext);

    // Decrypt the ciphertext in blocks of 3 letters
    for (size_t i = 0; i < textLength; i += 3) {
        decrypt(key, &ciphertext[i], &decryptedtext[i]);
    }
    decryptedtext[textLength] = '\0';  // Null-terminate the decrypted text string

    // Restore spaces in the decrypted text
    restoreSpaces(decryptedtext, specialChar);

    printf("Decrypted text: %s\n", decryptedtext);

    return 0;
}

