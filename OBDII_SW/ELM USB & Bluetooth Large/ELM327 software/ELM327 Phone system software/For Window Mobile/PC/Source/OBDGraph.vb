' OBD Graph
' Copyright (C) 2005 Dana Peters

' This program is free software; you can redistribute it and/or
' modify it under the terms of the GNU General Public License
' as published by the Free Software Foundation; either version 2
' the License, or (at your option) any later version.

' This program is distributed in the hope that it will be useful,
' but WITHOUT ANY WARRANTY; without even the implied warranty of
' MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
' GNU General Public License for more details.

' You should have received a copy of the GNU General Public License
' along with this program; if not, write to the Free Software
' Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

Imports System.IO
Imports System.Text
Imports System.Text.RegularExpressions
Imports SoftwareFX.ChartFX.Lite
Imports Microsoft.Win32

Public Class FOBDGraph
    Inherits System.Windows.Forms.Form

#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call

    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    Friend WithEvents PDBOpenFileDialog As System.Windows.Forms.OpenFileDialog
    Friend WithEvents MainMenu1 As System.Windows.Forms.MainMenu
    Friend WithEvents MenuItem1 As System.Windows.Forms.MenuItem
    Friend WithEvents LoadMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents ExitMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents MenuItem2 As System.Windows.Forms.MenuItem
    Friend WithEvents DataMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents MenuItem3 As System.Windows.Forms.MenuItem
    Friend WithEvents NormalizeMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents ColumnHeader1 As System.Windows.Forms.ColumnHeader
    Friend WithEvents ColumnHeader2 As System.Windows.Forms.ColumnHeader
    Friend WithEvents ColumnHeader3 As System.Windows.Forms.ColumnHeader
    Friend WithEvents SensorListView As System.Windows.Forms.ListView
    Friend WithEvents ColumnHeader4 As System.Windows.Forms.ColumnHeader
    Friend WithEvents MenuItem4 As System.Windows.Forms.MenuItem
    Friend WithEvents AboutMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents MenuItem5 As System.Windows.Forms.MenuItem
    Friend WithEvents StatusBar1 As System.Windows.Forms.StatusBar
    Friend WithEvents SensorContextMenu As System.Windows.Forms.ContextMenu
    Friend WithEvents SensorColorDialog As System.Windows.Forms.ColorDialog
    Friend WithEvents SelectAllMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents SelectNoneMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents LoadPalmMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents PalmDBOpenFileDialog As System.Windows.Forms.OpenFileDialog
    Friend WithEvents SensorChart As SoftwareFX.ChartFX.Lite.Chart
    Friend WithEvents ThreeDMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents GridMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents LegendMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents WideMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents ExportCsvMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents Splitter1 As System.Windows.Forms.Splitter
    Friend WithEvents RenameMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents ExportCsvFileDialog As System.Windows.Forms.SaveFileDialog
    Friend WithEvents ExportBitmapMenuItem As System.Windows.Forms.MenuItem
    Friend WithEvents ExportBitmapFileDialog As System.Windows.Forms.SaveFileDialog
    Friend WithEvents LoadPocketPcMenuItem As System.Windows.Forms.MenuItem
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Dim resources As System.Resources.ResourceManager = New System.Resources.ResourceManager(GetType(FOBDGraph))
        Me.PDBOpenFileDialog = New System.Windows.Forms.OpenFileDialog
        Me.MainMenu1 = New System.Windows.Forms.MainMenu
        Me.MenuItem1 = New System.Windows.Forms.MenuItem
        Me.LoadMenuItem = New System.Windows.Forms.MenuItem
        Me.LoadPalmMenuItem = New System.Windows.Forms.MenuItem
        Me.RenameMenuItem = New System.Windows.Forms.MenuItem
        Me.ExportCsvMenuItem = New System.Windows.Forms.MenuItem
        Me.ExportBitmapMenuItem = New System.Windows.Forms.MenuItem
        Me.MenuItem5 = New System.Windows.Forms.MenuItem
        Me.ExitMenuItem = New System.Windows.Forms.MenuItem
        Me.MenuItem2 = New System.Windows.Forms.MenuItem
        Me.DataMenuItem = New System.Windows.Forms.MenuItem
        Me.MenuItem3 = New System.Windows.Forms.MenuItem
        Me.NormalizeMenuItem = New System.Windows.Forms.MenuItem
        Me.ThreeDMenuItem = New System.Windows.Forms.MenuItem
        Me.GridMenuItem = New System.Windows.Forms.MenuItem
        Me.LegendMenuItem = New System.Windows.Forms.MenuItem
        Me.WideMenuItem = New System.Windows.Forms.MenuItem
        Me.MenuItem4 = New System.Windows.Forms.MenuItem
        Me.AboutMenuItem = New System.Windows.Forms.MenuItem
        Me.SensorChart = New SoftwareFX.ChartFX.Lite.Chart
        Me.SensorListView = New System.Windows.Forms.ListView
        Me.ColumnHeader1 = New System.Windows.Forms.ColumnHeader
        Me.ColumnHeader4 = New System.Windows.Forms.ColumnHeader
        Me.ColumnHeader2 = New System.Windows.Forms.ColumnHeader
        Me.ColumnHeader3 = New System.Windows.Forms.ColumnHeader
        Me.SensorContextMenu = New System.Windows.Forms.ContextMenu
        Me.SelectAllMenuItem = New System.Windows.Forms.MenuItem
        Me.SelectNoneMenuItem = New System.Windows.Forms.MenuItem
        Me.StatusBar1 = New System.Windows.Forms.StatusBar
        Me.SensorColorDialog = New System.Windows.Forms.ColorDialog
        Me.PalmDBOpenFileDialog = New System.Windows.Forms.OpenFileDialog
        Me.Splitter1 = New System.Windows.Forms.Splitter
        Me.ExportCsvFileDialog = New System.Windows.Forms.SaveFileDialog
        Me.ExportBitmapFileDialog = New System.Windows.Forms.SaveFileDialog
        Me.LoadPocketPcMenuItem = New System.Windows.Forms.MenuItem
        Me.SuspendLayout()
        '
        'PDBOpenFileDialog
        '
        Me.PDBOpenFileDialog.Filter = "OBD Data files|*.pdb;*.dat|All files|*.*"
        Me.PDBOpenFileDialog.Title = "Open OBD Data"
        '
        'MainMenu1
        '
        Me.MainMenu1.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.MenuItem1, Me.MenuItem2, Me.MenuItem3, Me.MenuItem4})
        '
        'MenuItem1
        '
        Me.MenuItem1.Index = 0
        Me.MenuItem1.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.LoadMenuItem, Me.LoadPalmMenuItem, Me.LoadPocketPcMenuItem, Me.RenameMenuItem, Me.ExportCsvMenuItem, Me.ExportBitmapMenuItem, Me.MenuItem5, Me.ExitMenuItem})
        Me.MenuItem1.Text = "File"
        '
        'LoadMenuItem
        '
        Me.LoadMenuItem.Index = 0
        Me.LoadMenuItem.Text = "Open OBD Data..."
        '
        'LoadPalmMenuItem
        '
        Me.LoadPalmMenuItem.Index = 1
        Me.LoadPalmMenuItem.Text = "Open OBD Data from Palm"
        '
        'RenameMenuItem
        '
        Me.RenameMenuItem.Index = 3
        Me.RenameMenuItem.Text = "Rename..."
        '
        'ExportCsvMenuItem
        '
        Me.ExportCsvMenuItem.Index = 4
        Me.ExportCsvMenuItem.Text = "Export OBD Data as CSV..."
        '
        'ExportBitmapMenuItem
        '
        Me.ExportBitmapMenuItem.Index = 5
        Me.ExportBitmapMenuItem.Text = "Export Graph as Bitmap..."
        '
        'MenuItem5
        '
        Me.MenuItem5.Index = 6
        Me.MenuItem5.Text = "-"
        '
        'ExitMenuItem
        '
        Me.ExitMenuItem.Index = 7
        Me.ExitMenuItem.Text = "Exit"
        '
        'MenuItem2
        '
        Me.MenuItem2.Index = 1
        Me.MenuItem2.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.DataMenuItem})
        Me.MenuItem2.Text = "View"
        '
        'DataMenuItem
        '
        Me.DataMenuItem.Index = 0
        Me.DataMenuItem.Text = "Data Table"
        '
        'MenuItem3
        '
        Me.MenuItem3.Index = 2
        Me.MenuItem3.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.NormalizeMenuItem, Me.ThreeDMenuItem, Me.GridMenuItem, Me.LegendMenuItem, Me.WideMenuItem})
        Me.MenuItem3.Text = "Graph"
        '
        'NormalizeMenuItem
        '
        Me.NormalizeMenuItem.Index = 0
        Me.NormalizeMenuItem.Text = "Normalize"
        '
        'ThreeDMenuItem
        '
        Me.ThreeDMenuItem.Index = 1
        Me.ThreeDMenuItem.Text = "3-Dimensional"
        '
        'GridMenuItem
        '
        Me.GridMenuItem.Index = 2
        Me.GridMenuItem.Text = "Grid Lines"
        '
        'LegendMenuItem
        '
        Me.LegendMenuItem.Index = 3
        Me.LegendMenuItem.Text = "Legend"
        '
        'WideMenuItem
        '
        Me.WideMenuItem.Index = 4
        Me.WideMenuItem.Text = "Wide Lines"
        '
        'MenuItem4
        '
        Me.MenuItem4.Index = 3
        Me.MenuItem4.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.AboutMenuItem})
        Me.MenuItem4.Text = "Help"
        '
        'AboutMenuItem
        '
        Me.AboutMenuItem.Index = 0
        Me.AboutMenuItem.Text = "About OBD Graph..."
        '
        'SensorChart
        '
        Me.SensorChart.AxisY.Visible = False
        Me.SensorChart.BottomGap = 8
        Me.SensorChart.Cluster = True
        Me.SensorChart.Dock = System.Windows.Forms.DockStyle.Fill
        Me.SensorChart.Gallery = SoftwareFX.ChartFX.Lite.Gallery.Lines
        Me.SensorChart.LeftGap = 8
        Me.SensorChart.Location = New System.Drawing.Point(200, 0)
        Me.SensorChart.MarkerShape = SoftwareFX.ChartFX.Lite.MarkerShape.None
        Me.SensorChart.Name = "SensorChart"
        Me.SensorChart.NSeries = 1
        Me.SensorChart.RightGap = 8
        Me.SensorChart.Size = New System.Drawing.Size(464, 315)
        Me.SensorChart.TabIndex = 1
        Me.SensorChart.TopGap = 8
        '
        'SensorListView
        '
        Me.SensorListView.Activation = System.Windows.Forms.ItemActivation.OneClick
        Me.SensorListView.CheckBoxes = True
        Me.SensorListView.Columns.AddRange(New System.Windows.Forms.ColumnHeader() {Me.ColumnHeader1, Me.ColumnHeader4, Me.ColumnHeader2, Me.ColumnHeader3})
        Me.SensorListView.ContextMenu = Me.SensorContextMenu
        Me.SensorListView.Dock = System.Windows.Forms.DockStyle.Left
        Me.SensorListView.HeaderStyle = System.Windows.Forms.ColumnHeaderStyle.None
        Me.SensorListView.Location = New System.Drawing.Point(0, 0)
        Me.SensorListView.Name = "SensorListView"
        Me.SensorListView.Scrollable = False
        Me.SensorListView.Size = New System.Drawing.Size(200, 315)
        Me.SensorListView.TabIndex = 3
        Me.SensorListView.View = System.Windows.Forms.View.Details
        '
        'ColumnHeader1
        '
        Me.ColumnHeader1.Text = ""
        Me.ColumnHeader1.Width = 18
        '
        'ColumnHeader4
        '
        Me.ColumnHeader4.Text = ""
        Me.ColumnHeader4.Width = 25
        '
        'ColumnHeader2
        '
        Me.ColumnHeader2.Text = ""
        Me.ColumnHeader2.Width = 136
        '
        'ColumnHeader3
        '
        Me.ColumnHeader3.Text = ""
        Me.ColumnHeader3.Width = 15
        '
        'SensorContextMenu
        '
        Me.SensorContextMenu.MenuItems.AddRange(New System.Windows.Forms.MenuItem() {Me.SelectAllMenuItem, Me.SelectNoneMenuItem})
        '
        'SelectAllMenuItem
        '
        Me.SelectAllMenuItem.Index = 0
        Me.SelectAllMenuItem.Text = "Select All"
        '
        'SelectNoneMenuItem
        '
        Me.SelectNoneMenuItem.Index = 1
        Me.SelectNoneMenuItem.Text = "Select None"
        '
        'StatusBar1
        '
        Me.StatusBar1.Location = New System.Drawing.Point(0, 315)
        Me.StatusBar1.Name = "StatusBar1"
        Me.StatusBar1.Size = New System.Drawing.Size(664, 22)
        Me.StatusBar1.TabIndex = 4
        '
        'SensorColorDialog
        '
        Me.SensorColorDialog.FullOpen = True
        '
        'PalmDBOpenFileDialog
        '
        Me.PalmDBOpenFileDialog.FileName = "OBDGaugeLog.pdb"
        Me.PalmDBOpenFileDialog.Filter = "OBD Data files|OBDGaugeLog.pdb"
        Me.PalmDBOpenFileDialog.Title = "Open OBD Data from HotSync"
        '
        'Splitter1
        '
        Me.Splitter1.Location = New System.Drawing.Point(200, 0)
        Me.Splitter1.Name = "Splitter1"
        Me.Splitter1.Size = New System.Drawing.Size(3, 315)
        Me.Splitter1.TabIndex = 5
        Me.Splitter1.TabStop = False
        '
        'ExportCsvFileDialog
        '
        Me.ExportCsvFileDialog.DefaultExt = "csv"
        Me.ExportCsvFileDialog.Filter = "CSV files|*.csv|All files|*.*"
        Me.ExportCsvFileDialog.Title = "Export OBD Data as CSV"
        '
        'ExportBitmapFileDialog
        '
        Me.ExportBitmapFileDialog.Filter = "BMP files|*.bmp|GIF files|*.gif|JPG files|*.jpg|TIFF files|*.tif|PNG files|*.png"
        Me.ExportBitmapFileDialog.Title = "Export Graph as Bitmap"
        '
        'LoadPocketPcMenuItem
        '
        Me.LoadPocketPcMenuItem.Index = 2
        Me.LoadPocketPcMenuItem.Text = "Open OBD Data from Pocket PC"
        '
        'FOBDGraph
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(664, 337)
        Me.Controls.Add(Me.Splitter1)
        Me.Controls.Add(Me.SensorChart)
        Me.Controls.Add(Me.SensorListView)
        Me.Controls.Add(Me.StatusBar1)
        Me.Icon = CType(resources.GetObject("$this.Icon"), System.Drawing.Icon)
        Me.Menu = Me.MainMenu1
        Me.Name = "FOBDGraph"
        Me.Text = "OBD Graph"
        Me.ResumeLayout(False)

    End Sub

#End Region

#Region " Types and Variables "

#If DEBUG Then
    Const Release As Boolean = False
#Else
    Const Release as Boolean = true
#End If

    Const APPNAME As String = "OBD Graph"

    Enum EParseState
        DatabaseStart
        DatabaseOptions
        RecordStart
        RecordOptions
        RecordData
    End Enum

    Dim mActivePidMap As BitArray
    Dim mDataTable, mNormalTable As DataTable
    Dim mSensorMax, mSensorMin As Int16()
    Dim mData As New FData
    Dim mColors(19) As Color
    Dim mBatchUpdate As Boolean
    Dim mFilename As String

#End Region

#Region " Windows Events "

    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Dim Index As Integer
        SensorChart.ClearData(ClearDataFlag.AllData)
        PDBOpenFileDialog.InitialDirectory = Path.GetDirectoryName(Application.ExecutablePath)
        ExportCsvFileDialog.InitialDirectory = Path.GetDirectoryName(Application.ExecutablePath)
        For Index = 0 To UBound(mColors)
            mColors(Index) = SensorChart.Series(Index).Color
            SensorChart.DataType(Index + 1) = DataType.NotUsed
        Next
    End Sub

    Private Sub LoadMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles LoadMenuItem.Click
        If PDBOpenFileDialog.ShowDialog(Me) = DialogResult.OK Then
            Select Case Path.GetExtension(PDBOpenFileDialog.FileName)
                Case ".pdb"
                    LoadPDB(PDBOpenFileDialog.FileName)
                Case ".dat"
                    LoadDAT(PDBOpenFileDialog.FileName)
            End Select
        End If
    End Sub

    Private Sub LoadPalmMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles LoadPalmMenuItem.Click
        Dim DbPath As String = GetPalmDbPath()
        If DbPath Is Nothing Then
            If MessageBox.Show("OBD data from the Palm could not be found." & vbCrLf & _
            "Ensure you have used the ""Start Recording"" option in OBD Gauge," & vbCrLf & _
            "and have performed a HotSync operation." & vbCrLf & vbCrLf & _
            "Do you want to browse for the OBD data file?", APPNAME, MessageBoxButtons.OKCancel, _
            MessageBoxIcon.Exclamation) = DialogResult.OK Then
                PalmDBOpenFileDialog.InitialDirectory = GetPalmPath()
                If PalmDBOpenFileDialog.ShowDialog(Me) = DialogResult.OK Then
                    LoadPDB(PalmDBOpenFileDialog.FileName)
                End If
            End If
        Else
            LoadPDB(CacheDb(DbPath))
        End If
    End Sub

    Private Sub LoadPocketPcMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles LoadPocketPcMenuItem.Click
        Dim DbPath As String = GetPocketPcDbPath()
        If DbPath Is Nothing Then
            MessageBox.Show("OBD data from the Pocket PC could not be found." & vbCrLf & _
            "Ensure you have used the ""Start Recording"" option in OBD Gauge," & vbCrLf & _
            "and have established an ActiveSync partnership. The synchronization" & vbCrLf & _
            "of files must also be enabled in Microsoft ActiveSync.", APPNAME, MessageBoxButtons.OK, _
            MessageBoxIcon.Exclamation)
        Else
            LoadDAT(CacheDb(DbPath))
        End If
    End Sub

    Private Sub RenameMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles RenameMenuItem.Click
        If mFilename Is Nothing Then
            Beep()
            Exit Sub
        End If
        Dim Rename As New FRename
        Dim Extension As String = Path.GetExtension(mFilename)
        Rename.RenameTextBox.Text = Path.GetFileNameWithoutExtension(mFilename)
        If Rename.ShowDialog(Me) = DialogResult.OK AndAlso Rename.RenameTextBox.Text.Length > 0 Then
            Dim NewFileName As String = Rename.RenameTextBox.Text & Extension
            Dim File As FileInfo = New FileInfo(mFilename)
            Dim NewFilePath As String = Path.Combine(Path.GetDirectoryName(mFilename), NewFileName)
            Try
                File.MoveTo(NewFilePath)
            Catch ex As Exception
                MessageBox.Show("Error" & vbCrLf & vbCrLf & ex.Message, APPNAME, MessageBoxButtons.OK, _
                MessageBoxIcon.Error)
                Exit Sub
            End Try
            mFilename = NewFileName
            SensorChart.Titles(0).Text = Path.GetFileNameWithoutExtension(mFilename)
        End If
    End Sub

    Private Sub ExportCsvMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ExportCsvMenuItem.Click
        If mDataTable Is Nothing Then
            Beep()
            Exit Sub
        End If
        ExportCsvFileDialog.FileName = Path.GetFileNameWithoutExtension(mFilename) & ".csv"
        If ExportCsvFileDialog.ShowDialog(Me) = DialogResult.OK Then
            ExportCSV(ExportCsvFileDialog.FileName)
        End If
    End Sub

    Private Sub ExportBitmapMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ExportBitmapMenuItem.Click
        Static ImageFormat() As Imaging.ImageFormat = { _
        Imaging.ImageFormat.Bmp, _
        Imaging.ImageFormat.Gif, _
        Imaging.ImageFormat.Jpeg, _
        Imaging.ImageFormat.Tiff, _
        Imaging.ImageFormat.Png}
        If mDataTable Is Nothing Then
            Beep()
            Exit Sub
        End If
        ExportBitmapFileDialog.FileName = Path.GetFileNameWithoutExtension(mFilename)
        If ExportBitmapFileDialog.ShowDialog(Me) = DialogResult.OK Then
            Dim SaveImage As Bitmap = New Bitmap(SensorChart.Width, SensorChart.Height)
            Dim g As Graphics = Graphics.FromImage(SaveImage)
            g.Clear(Color.Transparent)
            SensorChart.Paint(g, New Rectangle(0, 0, SaveImage.Width, SaveImage.Height), 0)
            SaveImage.Save(ExportBitmapFileDialog.FileName, ImageFormat(ExportBitmapFileDialog.FilterIndex - 1))
            SaveImage.Dispose()
        End If
    End Sub

    Private Sub ExitMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ExitMenuItem.Click
        Close()
    End Sub

    Private Sub DataMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles DataMenuItem.Click
        mData.Owner = Me
        mData.Show()
    End Sub

    Private Sub NormalizeMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles NormalizeMenuItem.Click
        NormalizeMenuItem.Checked = Not NormalizeMenuItem.Checked
        DisplayChart()
    End Sub

    Private Sub ThreeDMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ThreeDMenuItem.Click
        ThreeDMenuItem.Checked = Not ThreeDMenuItem.Checked
        SensorChart.Chart3D = ThreeDMenuItem.Checked
    End Sub

    Private Sub GridMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles GridMenuItem.Click
        GridMenuItem.Checked = Not GridMenuItem.Checked
        If GridMenuItem.Checked Then
            SensorChart.Grid = ChartGrid.Horz Or ChartGrid.Vert
        Else
            SensorChart.Grid = ChartGrid.None
        End If
    End Sub

    Private Sub LegendMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles LegendMenuItem.Click
        LegendMenuItem.Checked = Not LegendMenuItem.Checked
        SensorChart.SerLegBox = LegendMenuItem.Checked
    End Sub

    Private Sub WideMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles WideMenuItem.Click
        WideMenuItem.Checked = Not WideMenuItem.Checked
        If WideMenuItem.Checked Then
            SensorChart.LineWidth = 2
        Else
            SensorChart.LineWidth = 1
        End If
    End Sub

    Private Sub AboutMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles AboutMenuItem.Click
        Dim About As New FAbout
        About.ShowDialog(Me)
    End Sub

    Private Sub SelectAllMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles SelectAllMenuItem.Click
        Dim Item As ListViewItem
        mBatchUpdate = True
        For Each Item In SensorListView.Items
            Item.Checked = True
        Next
        mBatchUpdate = False
        DisplayChart()
    End Sub

    Private Sub SelectNoneMenuItem_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles SelectNoneMenuItem.Click
        Dim Item As ListViewItem
        mBatchUpdate = True
        For Each Item In SensorListView.Items
            Item.Checked = False
        Next
        mBatchUpdate = False
        DisplayChart()
    End Sub

    Private Sub SensorListView_ItemCheck(ByVal sender As Object, ByVal e As System.Windows.Forms.ItemCheckEventArgs) Handles SensorListView.ItemCheck
        If e.NewValue = CheckState.Checked Then
            SensorChart.DataType(e.Index + 1) = DataType.Value
        Else
            SensorChart.DataType(e.Index + 1) = DataType.NotUsed
        End If
        If Not mBatchUpdate Then
            DisplayChart()
        End If
    End Sub

    Private Sub SensorListView_MouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles SensorListView.MouseDown
        If e.Button = MouseButtons.Left Then
            Dim Item As ListViewItem = SensorListView.GetItemAt(5, e.Y)
            If Not Item Is Nothing Then
                Dim Index As Integer = SensorListView.Items.IndexOf(Item)
                If e.X > SensorListView.Columns(0).Width AndAlso _
                e.X < SensorListView.Columns(0).Width + SensorListView.Columns(1).Width + SensorListView.Columns(2).Width Then
                    Item.Checked = Not Item.Checked
                End If
            End If
        End If
    End Sub

    Private Sub SensorListView_MouseUp(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles SensorListView.MouseUp
        If e.Button = MouseButtons.Left Then
            Dim Item As ListViewItem = SensorListView.GetItemAt(5, e.Y)
            If Not Item Is Nothing Then
                Dim Index As Integer = SensorListView.Items.IndexOf(Item)
                If e.X > SensorListView.Columns(0).Width + SensorListView.Columns(1).Width + SensorListView.Columns(2).Width Then
                    SensorColorDialog.Color = mColors(Index)
                    If SensorColorDialog.ShowDialog(Me) = DialogResult.OK Then
                        mColors(Index) = SensorColorDialog.Color
                        SensorListView.Items(Index).SubItems(3).BackColor = SensorColorDialog.Color
                        UpdateColors()
                    End If
                End If
            End If
        End If
    End Sub

#End Region

#Region " Database File "

    Private Function GetPalmPath() As String
        Dim RegKey As RegistryKey = Registry.CurrentUser.OpenSubKey("Software\\U.S. Robotics\\Pilot Desktop\\Core")
        Dim PalmPath As String = String.Empty
        If Not RegKey Is Nothing Then
            PalmPath = RegKey.GetValue("Path", String.Empty).ToString
            RegKey.Close()
        End If
        Return PalmPath
    End Function

    Private Function GetPalmDbPath() As String
        Dim PalmPath As String = GetPalmPath()
        Dim DbFilePath As String
        If PalmPath.Length > 0 Then
            Dim Dirs() As String = Directory.GetDirectories(PalmPath)
            Dim Dir As String
            Dim DbLastWriteTime As DateTime
            For Each Dir In Dirs
                Dim FilePath As String = Dir & "\backup\OBDGaugeLog.pdb"
                If File.Exists(FilePath) Then
                    Dim LastWriteTime As DateTime = File.GetLastWriteTime(FilePath)
                    If DbFilePath Is Nothing OrElse LastWriteTime > DbLastWriteTime Then
                        DbFilePath = FilePath
                        DbLastWriteTime = LastWriteTime
                    End If
                End If
            Next
        End If
        Return DbFilePath
    End Function

    Private Function GetPocketPcDbPath() As String
        Dim DbFilePath As String
        Dim RegKey As RegistryKey = Registry.CurrentUser.OpenSubKey("Software\\Microsoft\\Windows CE Services\\Partners")
        If Not RegKey Is Nothing Then
            Dim SubKeyName As String
            Dim DbLastWriteTime As DateTime
            For Each SubKeyName In RegKey.GetSubKeyNames()
                Dim SubKey As RegistryKey = RegKey.OpenSubKey(SubKeyName & "\\Services\\Synchronization")
                Dim Dir As String = SubKey.GetValue("Briefcase Path", String.Empty).ToString
                If Dir.Length > 0 Then
                    Dim FilePath As String = Dir & "OBDGaugeLog.dat"
                    If File.Exists(FilePath) Then
                        Dim LastWriteTime As DateTime = File.GetLastWriteTime(FilePath)
                        If DbFilePath Is Nothing OrElse LastWriteTime > DbLastWriteTime Then
                            DbFilePath = FilePath
                            DbLastWriteTime = LastWriteTime
                        End If
                    End If
                End If
            Next
            RegKey.Close()
        End If
        Return DbFilePath
    End Function

    Private Function CacheDb(ByVal DbPath As String) As String
        Dim CacheDir As String = Path.GetDirectoryName(Application.ExecutablePath)
        Dim Extension As String = Path.GetExtension(DbPath)
        Dim DbInfo As FileInfo = New FileInfo(DbPath)
        Dim CachePath As String
        For Each CachePath In Directory.GetFiles(CacheDir, String.Format("*{0:s}", Extension))
            Dim CacheInfo As FileInfo = New FileInfo(CachePath)
            If CacheInfo.LastWriteTime = DbInfo.LastWriteTime Then
                If CacheInfo.Length = DbInfo.Length Then
                    Return CachePath
                End If
            End If
        Next
        Dim UniqueNumber As Integer
        Do
            UniqueNumber += 1
            CachePath = Path.Combine(CacheDir, String.Format("OBD Data {0:d2}{1:s}", UniqueNumber, Extension))
        Loop Until Not File.Exists(CachePath)
        DbInfo.CopyTo(CachePath)
        Return CachePath
    End Function

    Private Sub LoadPDB(ByVal DbPath As String)
        Try
            Dim PDBDecExe As String = Path.Combine(Path.GetDirectoryName(Application.ExecutablePath), "pdbdec.exe")
            Dim TempFile As String = Path.Combine(Path.GetTempPath(), Path.GetTempFileName())
            Try
                Shell(String.Format("""{0:s}"" -o ""{1:s}"" ""{2:s}""", PDBDecExe, TempFile, DbPath), _
                AppWinStyle.Hide, True)
            Catch ex As FileNotFoundException
                Throw New ApplicationException("Could not execute Palm database decompiler ""pdbdec.exe""")
            End Try
            mDataTable = Nothing
            mActivePidMap = New BitArray(32)
            ReadPDB(TempFile) 'calculate ActivePidMap
            mDataTable = New DataTable
            mNormalTable = New DataTable
            CreateDataColumns()
            ReadPDB(TempFile)
            File.Delete(TempFile)
            mData.SensorDataGrid.DataSource = New DataView(mDataTable)
            Dim Index As Integer
            For Index = 0 To SensorListView.Items.Count - 1
                SensorChart.DataType(Index + 1) = DataType.NotUsed
            Next
            SensorChart.Titles(0).Text = Path.GetFileNameWithoutExtension(DbPath)
            If SensorListView.Items.Count > 0 Then
                SensorListView.Items(0).Checked = True
            Else
                DisplayChart()
            End If
            mFilename = DbPath
        Catch ex As ApplicationException
            MessageBox.Show("Error " & vbCrLf & vbCrLf & ex.Message, _
            APPNAME, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception When Release
            MessageBox.Show("Error " & vbCrLf & vbCrLf & ex.Message, _
            APPNAME, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
    End Sub

    Private Sub LoadDAT(ByVal DbPath As String)
        Try
            mDataTable = Nothing
            mActivePidMap = New BitArray(32)
            ReadDAT(DbPath) 'calculate ActivePidMap
            mDataTable = New DataTable
            mNormalTable = New DataTable
            CreateDataColumns()
            ReadDAT(DbPath)
            mData.SensorDataGrid.DataSource = New DataView(mDataTable)
            Dim Index As Integer
            For Index = 0 To SensorListView.Items.Count - 1
                SensorChart.DataType(Index + 1) = DataType.NotUsed
            Next
            SensorChart.Titles(0).Text = Path.GetFileNameWithoutExtension(DbPath)
            If SensorListView.Items.Count > 0 Then
                SensorListView.Items(0).Checked = True
            Else
                DisplayChart()
            End If
            mFilename = DbPath
        Catch ex As ApplicationException
            MessageBox.Show("Error " & vbCrLf & vbCrLf & ex.Message, _
            APPNAME, MessageBoxButtons.OK, MessageBoxIcon.Error)
        Catch ex As Exception When Release
            MessageBox.Show("Error " & vbCrLf & vbCrLf & ex.Message, _
            APPNAME, MessageBoxButtons.OK, MessageBoxIcon.Error)
        End Try
    End Sub

    Private Sub ReadPDB(ByVal InFile As String)
        Dim InReader As New StreamReader(InFile)
        Dim ParseState As EParseState
        Dim HexValues(72) As Byte
        Dim HexIndex As Integer
        Dim HexRe As New Regex("   ((?<hex>[0-9a-f][0-9a-f]) )+")
        Dim FilenameRe As New Regex(" filename = ""(?<filename>.*)"";")
        While InReader.Peek >= 0
            Dim Line As String = InReader.ReadLine()
            Select Case ParseState
                Case EParseState.DatabaseStart
                    If Line = "file" Then
                        ParseState = EParseState.DatabaseOptions
                    End If
                Case EParseState.DatabaseOptions
                    If Line = "begin" Then
                        ParseState = EParseState.RecordStart
                    Else
                        Dim FilenameMatch As Match = FilenameRe.Match(Line)
                        If FilenameMatch.Success Then
                            Dim PDBFilename As String = FilenameMatch.Groups("filename").Captures(0).Value
                            If PDBFilename <> "OBDGaugeLog" Then
                                Throw New ApplicationException( _
                                "This file is not a Palm database created by OBD Gauge.")
                            End If
                        End If
                    End If
                Case EParseState.RecordStart
                    If Line = " record" Then
                        ParseState = EParseState.RecordOptions
                    End If
                Case EParseState.RecordOptions
                    If Line = " begin" Then
                        HexIndex = 0
                        ParseState = EParseState.RecordData
                    End If
                Case EParseState.RecordData
                    If Line = " end;" Then
                        If mDataTable Is Nothing Then
                            UpdateActiveMap(HexValues, HexIndex, False)
                        Else
                            AddDataRow(HexValues, HexIndex, False)
                        End If
                        ParseState = EParseState.RecordStart
                    Else
                        Dim HexMatch As Match = HexRe.Match(Line)
                        If HexMatch.Success Then
                            Dim i As Integer
                            For i = 0 To HexMatch.Groups("hex").Captures.Count - 1
                                HexValues(HexIndex) = Byte.Parse(HexMatch.Groups("hex").Captures(i).Value, _
                                Globalization.NumberStyles.AllowHexSpecifier)
                                HexIndex += 1
                            Next
                        End If
                    End If
            End Select
        End While
        InReader.Close()
    End Sub

    Private Sub ReadDAT(ByVal InFile As String)
        Dim InReader As New BinaryReader(New FileStream(InFile, FileMode.Open))
        Dim ParseState As EParseState
        Dim HexValues() As Byte
        Dim HexIndex As Integer
        While InReader.PeekChar >= 0
            Dim Length As Byte = InReader.ReadByte()
            HexValues = InReader.ReadBytes(Length)
            If mDataTable Is Nothing Then
                UpdateActiveMap(HexValues, HexValues.Length, True)
            Else
                AddDataRow(HexValues, HexValues.Length, True)
            End If
        End While
        InReader.Close()
    End Sub

    Private Sub ExportCSV(ByVal CsvPath As String)
        Dim CsvWriter As StreamWriter = New StreamWriter(CsvPath)
        Dim Column As DataColumn
        Dim FirstColumn As Boolean = True
        For Each Column In mDataTable.Columns
            If Not FirstColumn Then
                CsvWriter.Write(",")
            End If
            FirstColumn = False
            CsvWriter.Write(Column.ColumnName)
        Next
        CsvWriter.WriteLine()
        Dim Row As DataRow
        For Each Row In mDataTable.Rows
            Dim ColumnIndex As Integer
            For ColumnIndex = 0 To mDataTable.Columns.Count - 1
                If ColumnIndex > 0 Then
                    CsvWriter.Write(",")
                End If
                CsvWriter.Write(Row.Item(ColumnIndex))
            Next
            CsvWriter.WriteLine()
        Next
        CsvWriter.Close()
    End Sub

    Private Function BytesToUInt32(ByVal Data() As Byte, ByVal Start As Integer, ByVal LittleEndian As Boolean) As UInt32
        If LittleEndian Xor BitConverter.IsLittleEndian Then
            Dim Bytes(3) As Byte
            Bytes(3) = Data(Start)
            Bytes(2) = Data(Start + 1)
            Bytes(1) = Data(Start + 2)
            Bytes(0) = Data(Start + 3)
            Return BitConverter.ToUInt32(Bytes, 0)
        Else
            Return BitConverter.ToUInt32(Data, Start)
        End If
    End Function

    Private Function BytesToInt16(ByVal Data() As Byte, ByVal Start As Integer, ByVal LittleEndian As Boolean) As Int16
        If LittleEndian Xor BitConverter.IsLittleEndian Then
            Dim Bytes(1) As Byte
            Bytes(1) = Data(Start)
            Bytes(0) = Data(Start + 1)
            Return BitConverter.ToInt16(Bytes, 0)
        Else
            Return BitConverter.ToInt16(Data, Start)
        End If
    End Function

    Private Function BytesToBitArray(ByVal Data() As Byte, ByVal Start As Integer, ByVal LittleEndian As Boolean) As BitArray
        Dim Bytes(3) As Byte
        If LittleEndian Xor BitConverter.IsLittleEndian Then
            Bytes(0) = Data(Start + 3)
            Bytes(1) = Data(Start + 2)
            Bytes(2) = Data(Start + 1)
            Bytes(3) = Data(Start)
        Else
            Array.Copy(Data, Start, Bytes, 0, 4)
        End If
        Return New BitArray(Bytes)
    End Function

#End Region

#Region " Grid and Chart "

    Private Sub DisplayChart()
        UpdateColors()
        If NormalizeMenuItem.Checked Then
            SensorChart.AxisY.Visible = False
            SensorChart.DataSource = mNormalTable
        Else
            SensorChart.AxisY.Visible = True
            SensorChart.DataSource = mDataTable
        End If
    End Sub

    Private Sub UpdateColors()
        Dim Index As Integer
        Dim SeriesIndex As Integer
        For Index = 0 To UBound(mColors)
            If SensorChart.DataType(Index + 1) = DataType.Value Then
                SensorChart.Series(SeriesIndex).Color = mColors(Index)
                SeriesIndex += 1
            End If
        Next
    End Sub

    Public Sub CreateDataColumns()
        Dim Bit As Integer
        Dim ColumnNumber As Integer
        Dim Item As ListViewItem
        mDataTable.Columns.Add(New DataColumn("Time", GetType(DateTime)))
        mNormalTable.Columns.Add(New DataColumn("Time", GetType(DateTime)))
        Dim TextBoxColumn As DataGridTextBoxColumn
        TextBoxColumn = New DataGridTextBoxColumn
        TextBoxColumn.Format = "G"
        TextBoxColumn.FormatInfo = Nothing
        TextBoxColumn.HeaderText = "Time"
        TextBoxColumn.MappingName = "Time"
        TextBoxColumn.Width = 120
        mData.DataGridTableStyle1.GridColumnStyles.Clear()
        mData.DataGridTableStyle1.GridColumnStyles.Add(TextBoxColumn)
        SensorListView.Items.Clear()
        For Bit = 31 To 0 Step -1
            If mActivePidMap(Bit) Then
                Dim Pid As Integer = 32 - Bit
                Dim Name As String
                Select Case Pid
                    Case &H11
                        Name = "Throttle position"
                    Case &HC
                        Name = "Engine RPM"
                    Case &HD
                        Name = "Vehicle speed"
                    Case &H4
                        Name = "Calculated load value"
                    Case &HE
                        Name = "Ignition timing advance"
                    Case &HB
                        Name = "Intake air pressure"
                    Case &H10
                        Name = "Intake air flow rate"
                    Case &H6
                        Name = "Short term fuel trim"
                    Case &H7
                        Name = "Long term fuel trim"
                    Case &H8
                        Name = "Short term fuel trim 2"
                    Case &H9
                        Name = "Long term fuel trim 2"
                    Case &HF
                        Name = "Air temperature"
                    Case &H5
                        Name = "Coolent temperature"
                    Case &HA
                        Name = "Fuel pressure"
                    Case &H14
                        Name = "Oxygen Sensor 1"
                    Case &H15
                        Name = "Oxygen Sensor 2"
                    Case &H16
                        Name = "Oxygen Sensor 3"
                    Case &H17
                        Name = "Oxygen Sensor 4"
                    Case &H18
                        Name = "Oxygen Sensor 2-1"
                    Case &H19
                        Name = "Oxygen Sensor 2-2"
                    Case &H1A
                        Name = "Oxygen Sensor 2-3"
                    Case &H1B
                        Name = "Oxygen Sensor 2-4"
                    Case Else
                        Name = String.Empty
                End Select
                ColumnNumber += 1
                mDataTable.Columns.Add(New DataColumn(Name, GetType(Int16)))
                mNormalTable.Columns.Add(New DataColumn(Name, GetType(Int16)))
                TextBoxColumn = New DataGridTextBoxColumn
                TextBoxColumn.HeaderText = ColumnNumber.ToString
                TextBoxColumn.MappingName = Name
                TextBoxColumn.Width = 40
                mData.DataGridTableStyle1.GridColumnStyles.Add(TextBoxColumn)
                Item = New ListViewItem
                Item.SubItems.Add(ColumnNumber.ToString)
                Item.SubItems.Add(Name)
                Dim SubItem As ListViewItem.ListViewSubItem = Item.SubItems.Add(String.Empty)
                SubItem.BackColor = mColors(ColumnNumber - 1)
                Item.UseItemStyleForSubItems = False
                SensorListView.Items.Add(Item)
            End If
            ReDim mSensorMax(ColumnNumber - 1)
            ReDim mSensorMin(ColumnNumber - 1)
        Next
    End Sub

    Private Sub UpdateActiveMap(ByRef Data As Byte(), ByVal Length As Integer, ByVal LittleEndian As Boolean)
        Dim PidMap As BitArray = BytesToBitArray(Data, 4, LittleEndian)
        Dim Bit As Integer
        For Bit = 0 To 31
            If PidMap(Bit) Then
                mActivePidMap(Bit) = True
            End If
        Next
    End Sub

    Private Sub AddDataRow(ByRef Data As Byte(), ByVal Length As Integer, ByVal LittleEndian As Boolean)
        Dim Seconds As UInt32 = BytesToUInt32(Data, 0, LittleEndian)
        Dim RecordDate As DateTime = New DateTime(1904, 1, 1).AddSeconds(Convert.ToDouble(Seconds))
        Dim PidMap As BitArray = BytesToBitArray(Data, 4, LittleEndian)
        Dim Bit As Integer
        Dim ValueIndex, SensorIndex As Integer
        Dim SensorDataRow, SensorNormalRow As DataRow
        SensorDataRow = mDataTable.NewRow
        SensorNormalRow = mNormalTable.NewRow
        SensorDataRow(0) = RecordDate
        SensorNormalRow(0) = RecordDate
        For Bit = 31 To 0 Step -1
            If mActivePidMap(Bit) Then
                If PidMap(Bit) Then
                    Dim Pid As Integer = 32 - Bit
                    Dim Value As Int16 = BytesToInt16(Data, 8 + SensorIndex * 2, LittleEndian)
                    If Not mDataTable Is Nothing Then
                        If Value <> &H7FFF Then
                            If mSensorMax(ValueIndex) < Value Then
                                mSensorMax(ValueIndex) = Value
                            End If
                            If mSensorMin(ValueIndex) > Value Then
                                mSensorMin(ValueIndex) = Value
                            End If
                            SensorDataRow(ValueIndex + 1) = Value
                            If mSensorMax(ValueIndex) <> mSensorMin(ValueIndex) Then
                                SensorNormalRow(ValueIndex + 1) = (Value - mSensorMin(ValueIndex)) * 1000 _
                                / (mSensorMax(ValueIndex) - mSensorMin(ValueIndex))
                            Else
                                SensorNormalRow(ValueIndex + 1) = 0
                            End If
                        End If
                    End If
                    SensorIndex += 1
                End If
                ValueIndex += 1
            End If
        Next
        mDataTable.Rows.Add(SensorDataRow)
        mNormalTable.Rows.Add(SensorNormalRow)
    End Sub

#End Region

End Class
