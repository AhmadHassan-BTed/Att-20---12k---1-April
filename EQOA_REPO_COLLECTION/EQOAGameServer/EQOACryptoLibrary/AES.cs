using System.Security.Cryptography;

namespace AES
{
    public class AesCryptographyService
    {
        private Aes _aes;

        public AesCryptographyService(byte[] key)
        {
            _aes = Aes.Create();
            _aes.Key = key;

            _aes.IV = new byte[16] { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
        }
        public Memory<byte> Encrypt(Span<byte> data) => _aes.EncryptCbc(data, _aes.IV, PaddingMode.None);

        public Memory<byte> Decrypt(ReadOnlyMemory<byte> data) => _aes.DecryptCbc(data.Span, _aes.IV, PaddingMode.None);
    }
}

