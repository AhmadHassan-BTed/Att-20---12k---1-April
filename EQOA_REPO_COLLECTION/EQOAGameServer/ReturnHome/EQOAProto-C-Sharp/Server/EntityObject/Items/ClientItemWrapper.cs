namespace ReturnHome.Server.EntityObject.Items
{
    //Client stores items against a unique key when sending references to client
    //this helps to store that data as a reference for lookups
    public struct ClientItemWrapper
    {
        public Item item;
        public byte key;

        public ClientItemWrapper(Item i, byte k)
        {
            item = i;
            key = k;
        }
    }
}
