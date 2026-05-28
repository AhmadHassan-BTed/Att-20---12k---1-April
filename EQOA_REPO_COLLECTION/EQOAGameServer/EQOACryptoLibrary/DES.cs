using System.Security.Cryptography;

namespace EQOACryptoLibrary
{
    public class Des
    {
        private DES _des;

        public Des(byte[] key)
        {
            _des = DES.Create();
            _des.Key = key; 
            _des.IV = _des.Key;
        }

        public Memory<byte> Encrypt(Span<byte> data) => _des.EncryptCbc(data, _des.IV, PaddingMode.None);

        public Memory<byte> Decrypt(ReadOnlyMemory<byte> data) => _des.DecryptCbc(data.Span, _des.IV, PaddingMode.None);
    }
}
