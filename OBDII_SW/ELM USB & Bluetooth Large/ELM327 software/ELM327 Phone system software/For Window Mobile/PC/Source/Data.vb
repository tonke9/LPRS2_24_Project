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

Public Class FData
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
    Friend WithEvents SensorDataGrid As System.Windows.Forms.DataGrid
    Friend WithEvents StatusBar1 As System.Windows.Forms.StatusBar
    Friend WithEvents DataGridTableStyle1 As System.Windows.Forms.DataGridTableStyle
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.SensorDataGrid = New System.Windows.Forms.DataGrid
        Me.DataGridTableStyle1 = New System.Windows.Forms.DataGridTableStyle
        Me.StatusBar1 = New System.Windows.Forms.StatusBar
        CType(Me.SensorDataGrid, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'SensorDataGrid
        '
        Me.SensorDataGrid.CaptionText = "Sensor Data"
        Me.SensorDataGrid.CaptionVisible = False
        Me.SensorDataGrid.DataMember = ""
        Me.SensorDataGrid.Dock = System.Windows.Forms.DockStyle.Fill
        Me.SensorDataGrid.HeaderForeColor = System.Drawing.SystemColors.ControlText
        Me.SensorDataGrid.Location = New System.Drawing.Point(0, 0)
        Me.SensorDataGrid.Name = "SensorDataGrid"
        Me.SensorDataGrid.ReadOnly = True
        Me.SensorDataGrid.Size = New System.Drawing.Size(480, 251)
        Me.SensorDataGrid.TabIndex = 1
        Me.SensorDataGrid.TableStyles.AddRange(New System.Windows.Forms.DataGridTableStyle() {Me.DataGridTableStyle1})
        '
        'DataGridTableStyle1
        '
        Me.DataGridTableStyle1.DataGrid = Me.SensorDataGrid
        Me.DataGridTableStyle1.HeaderForeColor = System.Drawing.SystemColors.ControlText
        Me.DataGridTableStyle1.MappingName = ""
        '
        'StatusBar1
        '
        Me.StatusBar1.Location = New System.Drawing.Point(0, 251)
        Me.StatusBar1.Name = "StatusBar1"
        Me.StatusBar1.Size = New System.Drawing.Size(480, 22)
        Me.StatusBar1.TabIndex = 2
        '
        'FData
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(480, 273)
        Me.Controls.Add(Me.SensorDataGrid)
        Me.Controls.Add(Me.StatusBar1)
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.SizableToolWindow
        Me.Name = "FData"
        Me.ShowInTaskbar = False
        Me.Text = "Data Table"
        CType(Me.SensorDataGrid, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private Sub FData_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing
        Me.Hide()
        e.Cancel = True
    End Sub

End Class
