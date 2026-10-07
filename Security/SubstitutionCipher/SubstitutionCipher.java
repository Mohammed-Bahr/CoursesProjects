import java.util.Scanner;

/**
 * Substitution Cipher: every plaintext letter is replaced by the letter at the
 * same position in a 26-letter key (a -> key[0], b -> key[1], ... z -> key[25]).
 */
public class SubstitutionCipher {

    private static final String ALPHABET = "abcdefghijklmnopqrstuvwxyz";

    // ---------------------------------------------------------------
    // Key validation
    // ---------------------------------------------------------------
    // WHY: A substitution key must be a permutation of the alphabet:
    //  - exactly 26 characters (one replacement for each letter)
    //  - only letters
    //  - no repeats (otherwise two letters would encrypt to the same one
    //    and decryption would become ambiguous / impossible).
    static boolean isValidKey(String key) {
        if (key.length() != 26) return false;

        boolean[] seen = new boolean[26];
        for (char c : key.toLowerCase().toCharArray()) {
            if (c < 'a' || c > 'z') return false; // non-letter in key
            if (seen[c - 'a']) return false;      // duplicate letter
            seen[c - 'a'] = true;
        }
        return true;
    }

    // ---------------------------------------------------------------
    // Core transformation shared by encrypt() and decrypt()
    // ---------------------------------------------------------------
    // WHY: Encryption and decryption are the same operation with the two
    // alphabets swapped:
    //   encrypt: find letter in ALPHABET -> take the letter from KEY
    //   decrypt: find letter in KEY      -> take the letter from ALPHABET
    // Writing it once avoids duplicated (and bug-prone) code.
    private static String translate(String text, String from, String to) {
        StringBuilder result = new StringBuilder(); // StringBuilder: efficient, avoids creating many Strings

        for (char ch : text.toCharArray()) {
            if (Character.isLetter(ch)) {
                // Look up with lowercase so one table handles both cases.
                int index = from.indexOf(Character.toLowerCase(ch));

                if (index == -1) {
                    // Letter outside a-z (e.g. 'é'): not part of the cipher, keep as is.
                    result.append(ch);
                    continue;
                }

                char mapped = to.charAt(index);

                // WHY: Requirement says support upper AND lower case, so we
                // restore the original case of the input letter.
                result.append(Character.isUpperCase(ch) ? Character.toUpperCase(mapped) : mapped);
            } else {
                // WHY: Spaces, digits and punctuation must stay unchanged.
                result.append(ch);
            }
        }
        return result.toString();
    }

    // plaintext -> ciphertext
    public static String encrypt(String plaintext, String key) {
        return translate(plaintext, ALPHABET, key.toLowerCase());
    }

    // ciphertext -> plaintext (the reverse mapping: key -> alphabet)
    public static String decrypt(String ciphertext, String key) {
        return translate(ciphertext, key.toLowerCase(), ALPHABET);
    }

    // ---------------------------------------------------------------
    // Program entry: asks the user for input as the assignment requires
    // ---------------------------------------------------------------
    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        // nextLine() (not next()) so the message can contain spaces.
        System.out.print("Enter plaintext message: ");
        String plaintext = scanner.nextLine();

        // Keep asking until the key is valid, so the program never runs with a bad key.
        String key;
        while (true) {
            System.out.print("Enter substitution key (26 unique letters): ");
            key = scanner.nextLine().trim();
            if (isValidKey(key)) break;
            System.out.println("Invalid key! It must contain each of the 26 English letters exactly once.");
        }

        String ciphertext = encrypt(plaintext, key);
        String decrypted = decrypt(ciphertext, key);

        System.out.println("Ciphertext: " + ciphertext);
        // Round trip check: decrypting must give back the original text.
        System.out.println("Decrypted:  " + decrypted);

        scanner.close();
    }
}
