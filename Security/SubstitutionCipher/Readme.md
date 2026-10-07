Bahr, the program is done. I couldn't compile it here because this environment has no Java compiler, so I traced the example below by hand. Run `javac SubstitutionCipher.java && java SubstitutionCipher` to confirm.

**How it works**
- **Key:** letter `a` maps to `key[0]`, `b` to `key[1]`, and so on.
- **`encrypt()` and `decrypt()`:** both call one shared `translate()` method. Encryption looks a letter up in the alphabet and takes the matching key letter. Decryption does the reverse, so there is no duplicated logic.
- **Case:** each letter is looked up in lowercase and the original case is restored afterward.
- **Non-letters:** spaces, digits and punctuation are copied through unchanged.
- **Key validation:** the key must be exactly 26 letters with no repeats. A repeat would make two letters encrypt the same way, and decryption would become ambiguous. The program re-prompts until the key is valid.

**Example**
- Plaintext: `Hello World 123!`
- Key: `qwertyuiopasdfghjklzxcvbnm`
- Ciphertext: `Itssg Vgksr 123!`


**How to Compile and run:**
- `javac SubstitutionCipher.java`
- `java SubstitutionCipher`
