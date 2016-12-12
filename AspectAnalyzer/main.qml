import QtQuick 2.5
import QtQuick.Controls 1.4
import QtQuick.Dialogs 1.2

ApplicationWindow {
    id: aspectanalyzerwindow
    visible: true
    width: 640
    height: 480
    color: "#475542"
    title: qsTr("Aspect Analyzer")

    signal btnAddTaskSignal(int id)
    signal btnLoadFromFileSignal()
    signal btnLoadFromDatabaseSignal()
    signal btnTuneSignal()
    signal btnPlaySignal()
    signal btnPauseSignal()
    signal btnStopSignal()

    TabView {
        id: tabView1
        x: 0
        y: 0
        width: 640
        height: 480
        currentIndex: 0
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.fill: parent;

        Tab {
            id: experiment
            clip: false
            //anchors.fill: parent;
            title: "Experiment"
            source: "Tab1.qml"
            Rectangle { id: rectangle1; color: "#475542" ; anchors.fill: parent; border.color: "#475542"

                Label {
                    id: lblMatrixDesc
                    x: 374
                    y: 42
                    width: 44
                    height: 22
                    text: qsTr("No matrix loaded")
                    font.bold: true
                    font.pointSize: 12


                }

                Label {
                    id: lblLoadedMatrix
                    x: 374
                    y: 13
                    text: qsTr("Loaded matrix: ")
                    font.bold: true
                    font.pointSize: 12
                }

                Label {
                    id: lblDbID
                    x: 374
                    y: 75
                    width: 44
                    height: 22
                    text: qsTr("DB ID: No data")
                    font.bold: true
                    font.pointSize: 12
                    textFormat: Text.AutoText
                }

                ComboBox {
                    id: method
                    x: 374
                    y: 98
                    width: 225
                    height: 33
                    model: [ "PLSA", "Least Square Error", "Kullback-Liebler", "nsKullback-Liebler" ]
                }

                Image {
                    id: matrixPrev
                    x: 0
                    y: 0
                    width: 300
                    height: 300
                    anchors.bottomMargin: 68
                    anchors.leftMargin: 22
                    anchors.rightMargin: 313
                    anchors.topMargin: 98
                    anchors.fill: parent
                    fillMode: Image.PreserveAspectFit
                    opacity: 1
                    z: 2
                    source: "Graphics/no_image.png"
                }

                Label {
                    id: lblMatrixSize
                    x: 22
                    y: 13
                    text: qsTr("Matrix size: ? x ?")
                    font.pointSize: 12
                    font.bold: true
                }

                Label {
                    id: lblMatrixRange
                    x: 22
                    y: 51
                    text: qsTr("Values range: < ? - ? >")
                    font.bold: true
                    font.pointSize: 12
                }

                Label {
                    id: lblFile
                    x: 428
                    y: 188
                    text: qsTr("File")
                    font.pointSize: 12
                    font.bold: true
                }

                Label {
                    id: lblDatabse
                    x: 521
                    y: 188
                    text: qsTr("Database")
                    font.bold: true
                    font.pointSize: 12
                }

                Image {
                    id: btnLoadFromFile
                    x: 372
                    y: 175
                    width: 48
                    height: 48
                    source: "Graphics/file.png"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {

                            btnLoadFromFileSignal();

                        }
                    }
                }

                Image {
                    id: btnLoadFromDatabase
                    x: 467
                    y: 175
                    width: 48
                    height: 48
                    source: "Graphics/database.png"
                    MouseArea{
                        anchors.fill: parent
                        onClicked: {

                            btnLoadFromDatabaseSignal();

                        }
                    }
                }

                Label {
                    id: lblLoadMatrixFrom
                    x: 372
                    y: 147
                    text: qsTr("Load matrix from:")
                    font.pointSize: 12
                    font.bold: true
                }

                Image {
                    id: btnTune
                    x: 372
                    y: 242
                    width: 48
                    height: 48
                    source: "Graphics/tune.png"

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {

                            btnTuneSignal();

                        }
                    }
                }

                Image {
                    id: btnAddTask
                    x: 372
                    y: 309
                    width: 48
                    height: 48
                    source: "Graphics/add.png"

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {

                            btnAddTaskSignal(method.currentIndex);

                        }
                    }
                }

                Label {
                    id: lblTune
                    x: 426
                    y: 255
                    text: qsTr("Tune parameters")
                    font.bold: true
                    font.pointSize: 12
                }

                Label {
                    id: lblAddTask
                    x: 426
                    y: 322
                    text: qsTr("Add task")
                    font.pointSize: 12
                    font.bold: true
                }

                Connections {
                    target: lblreceiver
                    ignoreUnknownSignals: true
                    onSetMatrixLabels : {
                        lblMatrixDesc.text = text
                        lblMatrixSize.text = "Matrix size: " + nr + " x " + nc
                        lblMatrixRange.text = "Values range: < " + min + " ; " + max + " >"
                        lblDbID.text = "DB ID: " + dbID
                    }

                }



            }
        }

        Tab {
            id: worker
            visible: true
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            antialiasing: true
            transformOrigin: Item.Center
            title: "Worker"
            source: "Tab2.qml"
            Rectangle { id: rectangle2; color: "#475542" ; border.color: "#475542"

                Image {
                    id: imgRunning
                    x: 48
                    y: 71
                    width: 48
                    height: 48
                    source: "Graphics/running.png"
                }

                Image {
                    id: imgInQueue
                    x: 48
                    y: 153
                    width: 48
                    height: 48
                    source: "Graphics/queue.png"
                }

                Label {
                    id: lblInQueue
                    x: 122
                    y: 166
                    text: qsTr("0 tasks in queue")
                    font.bold: true
                    font.pointSize: 12
                }

                Label {
                    id: lblRunning
                    x: 124
                    y: 84
                    text: qsTr("0 tasks runinng")
                    font.bold: true
                    font.pointSize: 12
                }

                Image {
                    id: btnPlay
                    x: 48
                    y: 286
                    width: 48
                    height: 48
                    source: "Graphics/play.png"

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {

                            btnPlaySignal();

                        }
                    }
                }

                Image {
                    id: btnPause
                    x: 146
                    y: 286
                    width: 48
                    height: 48
                    source: "Graphics/pause.png"

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {

                            btnPauseSignal();

                        }
                    }
                }

                Image {
                    id: btnStop
                    x: 244
                    y: 286
                    width: 48
                    height: 48
                    source: "Graphics/stop.png"

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {

                            btnStopSignal();

                        }
                    }
                }

                Connections {
                    target: lblTasks
                    ignoreUnknownSignals: true

                    onSetTasksLabels : {
                        lblRunning.text = running + " tasks running"
                        lblInQueue.text = inqueue + " tasks in queue"
                    }
                }

            }
        }

        Tab {
            id: data
            visible: true
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            antialiasing: true
            transformOrigin: Item.Center
            title: "Data"
            source: "Tab3.qml"
            Rectangle { color: "#475542" ; border.color: "#475542"

            }
        }

        Tab {
            id: settings
            visible: true
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            antialiasing: true
            transformOrigin: Item.Center
            title: "Settings"
            source: "Tab4.qml"
            Rectangle { color: "#475542" ; border.color: "#475542"

            }
        }
    }
}
