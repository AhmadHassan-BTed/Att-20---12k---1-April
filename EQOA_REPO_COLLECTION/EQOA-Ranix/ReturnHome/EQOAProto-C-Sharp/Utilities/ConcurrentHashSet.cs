using System;
using System.Collections;
using System.Collections.Generic;
using System.Threading;

namespace ReturnHome.Utilities
{
    public class ConcurrentHashSet<T> : ICollection<T>, IEnumerable<T>, IReadOnlyCollection<T>, ICollection, IEnumerable where T : notnull
    {
        private readonly HashSet<T> _hashSet = new();
        private readonly ReaderWriterLockSlim _lock = new();

        public int Count
        {
            get
            {
                try
                {
                    _lock.EnterReadLock();

                    return _hashSet.Count;
                }
                finally
                {
                    if (_lock.IsReadLockHeld)
                        _lock.ExitReadLock();
                }
            }
        }

        public bool IsReadOnly => false;

        public bool IsSynchronized => throw new NotImplementedException();

        public object SyncRoot => throw new NotImplementedException();

        public void Add(T item)
        {
            try
            {
                _lock.EnterWriteLock();
                _hashSet.Add(item);
            }
            finally
            {
                if (_lock.IsWriteLockHeld)
                    _lock.ExitWriteLock();
            }
        }

        public void Clear()
        {
            try
            {
                _lock.EnterWriteLock();
                _hashSet.Clear();
            }
            finally
            {
                if (_lock.IsWriteLockHeld)
                    _lock.ExitWriteLock();
            }
        }

        public bool Contains(T item)
        {
            try
            {
                _lock.EnterReadLock();

                return _hashSet.Contains(item);
            }
            finally
            {
                if (_lock.IsReadLockHeld)
                    _lock.ExitReadLock();
            }
        }

        public void CopyTo(T[] array, int index)
        {
            if (index < 0)
                throw new ArgumentOutOfRangeException(nameof(index));

            HashSet<T> hashSet = ToHashSet();
            int count = hashSet.Count;

            if (index > array.Length - count)
                throw new ArgumentException("Destination array is not long enough to copy all the items in the collection. Check array index and length.");

            hashSet.CopyTo(array, index);
        }

        public void CopyTo(Array array, int index)
        {
            if (array is T[] a)
            {
                CopyTo(a, index);
                return;
            }

            ToArray().CopyTo(array, index);
        }

        public IEnumerator<T> GetEnumerator() => ToHashSet().GetEnumerator();

        public bool Remove(T item)
        {
            try
            {
                _lock.EnterWriteLock();

                return _hashSet.Remove(item);
            }
            finally
            {
                if (_lock.IsWriteLockHeld)
                    _lock.ExitWriteLock();
            }
        }

        IEnumerator IEnumerable.GetEnumerator() => GetEnumerator();

        public T[] ToArray()
        {
            HashSet<T> hashSet = ToHashSet();
            int count = hashSet.Count;

            T[] result = new T[count];

            using HashSet<T>.Enumerator e = hashSet.GetEnumerator();

            int i = 0;

            while (e.MoveNext())
                result[i++] = e.Current;

            return result;
        }

        public HashSet<T> ToHashSet()
        {
            HashSet<T> result;

            try
            {
                _lock.EnterReadLock();
                result = new HashSet<T>(_hashSet, _hashSet.Comparer);
            }
            finally
            {
                if (_lock.IsReadLockHeld)
                    _lock.ExitReadLock();
            }

            return result;
        }

        public bool TryAdd(T item)
        {
            if (Contains(item))
                return false;

            try
            {
                _lock.EnterWriteLock();

                if (_hashSet.Contains(item))
                    return false;

                _hashSet.Add(item);

                return true;
            }
            finally
            {
                if (_lock.IsWriteLockHeld)
                    _lock.ExitWriteLock();
            }
        }

        public bool TryRemove(T item)
        {
            if (!Contains(item))
                return false;

            try
            {
                _lock.EnterWriteLock();

                if (!_hashSet.Contains(item))
                    return false;

                _hashSet.Remove(item);

                return true;
            }
            finally
            {
                if (_lock.IsWriteLockHeld)
                    _lock.ExitWriteLock();
            }
        }
    }
}
