namespace ReturnHome.Server.EntityObject.Grouping
{
    public enum GroupActionEnum : byte
    {

        //InvitationRelated
        DeclinedInvitation = 1,
        AlreadyInAGroup = 2,
        RecipientNotFound = 3,
        IsIgnoringYou = 4,
        IsStillLoading = 5,

        //GroupActions
        LeaveGroup = 0,
        GroupLeaderEjectedYou = 1,
        GroupDisbanded = 2,
        NotInvitedToThisGroup = 3,
        GroupIsFull = 4,

    }
}
