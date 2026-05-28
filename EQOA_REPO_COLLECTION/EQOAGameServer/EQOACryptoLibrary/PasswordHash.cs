using System.Security.Cryptography;

namespace EQOACryptoLibrary
{
    public static class PasswordHash
    {
        public const int SaltByteSize = 24;
        public const int HashByteSize = 20; // to match the size of the PBKDF2-HMAC-SHA-1 hash 
        public const int Pbkdf2Iterations = 1000;
        public const int IterationIndex = 0;
        public const int SaltIndex = 1;
        public const int Pbkdf2Index = 2;

        public static string HashPassword(Memory<byte> password)
        {
            var cryptoProvider = new RNGCryptoServiceProvider();
            byte[] salt = new byte[SaltByteSize];
            cryptoProvider.GetBytes(salt);

            var hash = GetPbkdf2Bytes(password, salt, Pbkdf2Iterations, HashByteSize);
            return Pbkdf2Iterations + ":" +
                   Convert.ToBase64String(salt) + ":" +
                   Convert.ToBase64String(hash);
        }

        public static bool ValidatePassword(Memory<byte> password, string correctHash)
        {
            char[] delimiter = { ':' };
            var split = correctHash.Split(delimiter);
            var iterations = Int32.Parse(split[IterationIndex]);
            var salt = Convert.FromBase64String(split[SaltIndex]);
            Span<byte> hash = Convert.FromBase64String(split[Pbkdf2Index]);

            Span<byte> testHash = GetPbkdf2Bytes(password, salt, iterations, hash.Length);
            return testHash.SequenceEqual(hash);
        }

        private static byte[] GetPbkdf2Bytes(Memory<byte> password, byte[] salt, int iterations, int outputBytes)
        {
            var pbkdf2 = new Rfc2898DeriveBytes(password.ToArray(), salt, iterations);
            return pbkdf2.GetBytes(outputBytes);
        }
    }
}
