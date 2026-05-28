using AES;

namespace EQOACryptoLibrary
{
    public class CryptoLibrary
    {
        private CryptoOptions _crypto;
        private AesCryptographyService _aes = null;
        private Des _des = null;
        public CryptoLibrary(CryptoOptions cryp, byte[] key)
        {
            _crypto = cryp;
            if(cryp == CryptoOptions.DES)
                _des = new Des(key);

            if(cryp == CryptoOptions.AES)
                _aes = new AesCryptographyService(key);
        }

        public Memory<byte> Decrypt(ReadOnlyMemory<byte> memory)
        {
            Memory<byte> result;
            if (_crypto == CryptoOptions.DES)
                result = _des.Decrypt(memory);

            else
                result = _aes.Decrypt(memory);

            return result = result.Slice(0, result.Span.IndexOf<byte>(0x00));
        }

        public Memory<byte> Encrypt(Memory<byte> memory)
        {
            if (_crypto == CryptoOptions.DES)
                return _des.Encrypt(memory.Span);

            else
                return _aes.Encrypt(memory.Span);
        }
    }
}