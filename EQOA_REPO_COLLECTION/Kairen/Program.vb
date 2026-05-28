Module Program
    Public ReadOnly ProgramVersionNumber As String = "4.0.0.0"
    Public CurrentUserName As String
    Public Folders As New classFolders
    Public Files As New classFiles
    Friend Class classFolders
        Public ReadOnly AppData As String = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData) & "\Kairen4\"
        Public ReadOnly AccountsFolder As String = AppData & "Account Data\"
        Public CurrentAccount As String
    End Class
    Friend Class classFiles
        Public WithEvents ProgramSettings As New Class_TextFile(Folders.AppData, "ProgramSettings.txt", True)
        Public WithEvents UserSettings As Class_TextFile
    End Class
End Module
