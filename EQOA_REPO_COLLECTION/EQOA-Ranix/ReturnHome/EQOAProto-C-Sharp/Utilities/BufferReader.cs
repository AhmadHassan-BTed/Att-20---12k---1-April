// Copyright (C) 2022 Verant Team
//
// This file is part of Verant.
//
// Verant is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Verant is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with Verant. If not, see <https://www.gnu.org/licenses/>.

using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace ReturnHome.Utilities
{

    public ref struct BufferReader
    {
        private readonly ReadOnlySpan<byte> _buffer;
        private int _position;
        private int _length;
        /// <summary>
        /// Creates a <see cref="BufferReader" /> over a <see cref="ReadOnlySpan{T}" />.
        /// </summary>
        /// <remarks><typeparamref name="T" /> is <see langword="byte" /></remarks>
        /// <param name="buffer" />
        public BufferReader(ReadOnlySpan<byte> buffer)
        {
            _buffer = buffer;
            _position = 0;
            _length = buffer.Length;
        }

        /// <summary>
        /// Gets the underlying <see cref="ReadOnlySpan{T}" />.
        /// </summary>
        /// <remarks><typeparamref name="T" /> is <see langword="byte" /></remarks>
        public readonly ReadOnlySpan<byte> Buffer => _buffer;

        /// <summary>
        /// Gets or sets the position of the <see cref="BufferReader" />.
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
        /// Gets Length of the <see cref="BufferReader" />.
        /// </summary>
        public int Length
        {
            get => _length;
        }

        /// <summary>
        /// Gets the count of bytes remaining to be read from the <see cref="BufferReader" />.
        /// </summary>
        public int Remaining => _buffer.Length - Position;

        /// <summary>
        /// Advances the position of the <see cref="BufferReader" />.
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
        /// Reads a <typeparamref name="T" /> value from the <see cref="BufferReader" /> and advances its position.
        /// </summary>
        /// <remarks><typeparamref name="T" /> is <see langword="unmanaged" /></remarks>
        /// <typeparam name="T" />
        /// <returns><typeparamref name="T" /></returns>
        /// <exception cref="InvalidOperationException" />
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public unsafe T Read<T>() where T : unmanaged
        {
            int size = sizeof(T);

            if (size > Remaining)
                throw new InvalidOperationException($"{nameof(size)} exceeds the count of bytes remaining to be read from the {nameof(BufferReader)}");

            ReadOnlySpan<byte> slice = _buffer.Slice(Position, size);
            T result = MemoryMarshal.Read<T>(slice);
            Advance(size);
            return result;
        }

        /// <summary>
        /// Reads a <typeparamref name="T" />[] from the <see cref="BufferReader" /> and advances its position.
        /// </summary>
        /// <remarks><typeparamref name="T" /> is <see langword="unmanaged" /></remarks>
        /// <typeparam name="T" />
        /// <param name="count" />
        /// <returns><typeparamref name="T" />[]</returns>
        /// <exception cref="ArgumentOutOfRangeException" />
        /// <exception cref="InvalidOperationException" />
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public unsafe T[] ReadArray<T>(int count) where T : unmanaged
        {
            if (count < 0)
                throw new ArgumentOutOfRangeException(nameof(count));

            int size = sizeof(T) * count;

            if (size > Remaining)
                throw new InvalidOperationException($"{nameof(size)} exceeds the count of bytes remaining to be read from the {nameof(BufferReader)}");

            ReadOnlySpan<byte> slice = _buffer.Slice(Position, size);
            T[] result = MemoryMarshal.Cast<byte, T>(slice).ToArray();
            Advance(size);
            return result;
        }

        /// <summary>
        /// Reads a 7-bit encoded <see langword="long" /> value from the <see cref="BufferReader" /> and advances its position.
        /// </summary>
        /// <returns><see langword="long" /></returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public long Read7BitEncodedInt64()
        {
            ulong value = Read7BitEncodedUInt64();
            long result = (long)(value >> 1);

            if ((value & 1) != 0)
                result = ~result;

            return result;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public uint ReadUint24()
        {
            if (3 > Remaining)
                throw new InvalidOperationException($"3 exceeds the count of bytes remaining to be read from the {nameof(BufferReader)}");

            return (uint)(_buffer[_position++] << 16 | _buffer[_position++] << 8 | _buffer[_position++]);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public string ReadString(Encoding encoding, int size)
        {
            if (encoding is null)
                throw new ArgumentNullException(nameof(encoding));

            if (size < 0)
                throw new ArgumentOutOfRangeException(nameof(size));

            if (encoding == Encoding.Unicode)
                size <<= 1;

            if (size > Remaining)
                throw new InvalidOperationException($"size of {typeof(string)} exceeds the count of bytes remaining to be read from the {nameof(BufferReader)}");

            ReadOnlySpan<byte> slice = _buffer.Slice(_position, size);
            string result;

            if (encoding == Encoding.Unicode)
                result = Encoding.Unicode.GetString(slice);
            else
                result = Encoding.UTF8.GetString(slice);

            Advance(size);
            return result;
        }

        /// <summary>
        /// Reads a 7-bit encoded <see langword="ulong" /> value from the <see cref="BufferReader" /> and advances its position.
        /// </summary>
        /// <returns><see langword="ulong" /></returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public ulong Read7BitEncodedUInt64()
        {
            int i = 0;
            ulong result = 0;

            while (i < 64)
            {
                byte value = Read<byte>();
                result |= (uint)((value & 0x7F) << i);

                if ((value & 0x80) == 0)
                    break;

                i += 7;
            }

            return result;
        }

        /// <summary>
        /// Rewinds the position of the <see cref="BufferReader" />.
        /// </summary>
        /// <param name="count" />
        /// <exception cref="ArgumentOutOfRangeException" />
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public void Rewind(int count)
        {
            if (count < 0 || count > Position)
                throw new ArgumentOutOfRangeException(nameof(count));

            Position -= count;
        }
    }
}
