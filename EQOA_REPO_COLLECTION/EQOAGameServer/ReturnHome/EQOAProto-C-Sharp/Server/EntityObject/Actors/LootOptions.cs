namespace ReturnHome.Server.EntityObject.Actors
{
    public enum LootOptions : byte
    {
        CorpseBeingLootedByAnotherPlayer = 1,
        EntityNotLootable = 2,
        CorpseBelongsToAnotherPlayerOrGroup = 3
    }
}
