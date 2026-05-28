using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace ReturnHome.Utilities
{
    public ref struct BufferWriter
    {
        private Span<byte> _buffer;
        private int _position;
        private int _length;

        public BufferWriter(Span<byte> buffer)
        {
            _buffer = buffer;
            _length = _buffer.Length;
            _position = 0;
        }

        /// <summary>
        /// Gets the underlying <see cref="ReadOnlySpan{T}" />.
        /// </summary>
        /// <remarks><typeparamref name="T" /> is <see langword="byte" /></remarks>
        public Span<byte> Span => _buffer;

        /// <summary>
        /// Gets Length of the <see cref="BufferWriter" />.
        /// </summary>
        public int Length
        {
            get => _length;
        }

        /// <summary>
        /// Gets or sets the position of the <see cref="BufferWriter" />.
        /// </summary>
        public int Position
        {
            get => _position;
            set
            {
                if (value < 0 || value > _buffer.Length)
                    throw new ArgumentOutOfRangeException(nameof(value));

                _position = value;
            }
        }

        /// <summary>
        /// Gets the count of bytes remaining to be written to the <see cref="BufferWriter" />.
        /// </summary>
        public int Remaining => _buffer.Length - _position;

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public unsafe void Write(ReadOnlySpan<byte> val)
        {
            int size = val.Length;

            if (size > Remaining)
                throw new InvalidOperationException($"{nameof(size)} exceeds the count of bytes remaining to be wrote to the {nameof(BufferWriter)}");

            val.CopyTo(_buffer[Position..]);
            Advance(size);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public unsafe void Write<T>(T val) where T : unmanaged
        {
            int size = sizeof(T);

            if (size > Remaining)
                throw new InvalidOperationException($"{nameof(size)} exceeds the count of bytes remaining to be wrote to the {nameof(BufferWriter)}");

            MemoryMarshal.Write(_buffer[Position..], ref val);
            Advance(size);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public void WriteString(Encoding encoding, string str)
        {
            int size = str.Length;

            if (encoding is null)
                throw new ArgumentNullException(nameof(encoding));

            if (size < 0)
                throw new ArgumentOutOfRangeException(nameof(size));

            if (size > Remaining)
                throw new InvalidOperationException($"size of {typeof(string)} exceeds the count of bytes remaining to be wrote to the {nameof(BufferWriter)}");

            Write(size);

            if (Encoding.Unicode == encoding)
                size <<= 1;

            if (encoding == Encoding.Unicode)
                Encoding.Unicode.GetBytes(str).CopyTo(_buffer[Position..]);

            else
                Encoding.UTF8.GetBytes(str).CopyTo(_buffer[Position..]);

            Advance(size);
        }

        /// <summary>
        /// Writes Message size to the <see cref="BufferWriter" />.
        /// </summary>
        /// <param name="count" />
        public void WriteSize(int size)
        {
            if(size > 255)
            {
                Write((byte)0xFF);
                Write((ushort)size);
            }

            else
                Write((byte)size);
        }

        /// <summary>
        /// Advances the position of the <see cref="BufferWriter" />.
        /// </summary>
        /// <param name="count" />
        /// <exception cref="ArgumentOutOfRangeException" />
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public void Advance(int count)
        {
            if (count < 0 || count > Remaining)
                throw new ArgumentOutOfRangeException(nameof(count));

            Position += count;
        }

        /// <summary>
        /// Writes a 7-bit encoded <see langword="ulong" /> value from the <see cref="BufferWriter" /> and advances its position.
        /// </summary>
        /// <returns><see langword="ulong" /></returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public void Write7BitEncodedUInt64(ulong val)
        {
            byte b;

            b = (byte)(val & 0xFF);
            do
            {
                val = val >> 7;
                b = (byte)(b & 0x7f);
                if (val != 0)
                    b = (byte)(b | 0x80);

                _buffer[_position] = (byte)b;
                Advance(1);
                b = (byte)(val & 0xff);
            } while (val != 0);
        }

        /// <summary>
        /// Writes a 7-bit encoded <see langword="long" /> value from the <see cref="BufferWriter" /> and advances its position.
        /// </summary>
        /// <returns><see langword="long" /></returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public void Write7BitEncodedInt64(int val)
        {
            uint uVar1;
            uVar1 = (uint)val;
            uVar1 = val < 0 ? ((~uVar1) << 1) + 1 : uVar1 <<= 1;

            Write7BitEncodedUInt64(uVar1);
        }
    }
}
