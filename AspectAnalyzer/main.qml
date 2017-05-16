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
    signal btnPlaySignal()
    signal btnPauseSignal()
    signal btnStopSignal()
    signal btnCustomSignal(string mode)

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

                Image {
                    id: btnAddTask
                    width: 48
                    height: 48
                    x: 10
                    y: 10
                    transformOrigin: Item.TopLeft
                    source: "Graphics/add.png"

                    Text {
                        id: addlabel
                        text: "Add new tasks"
                        x: parent.x + 48
                        y: 10
                        font.bold: true
                        font.pointSize: 12
                    }

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {

                            btnAddTaskSignal(1);

                        }
                    }
                }

                Image {
                    id: imgRunning
                    x: 10
                    y: 98
                    width: 48
                    height: 48
                    source: "Graphics/running.png"

                    Text {
                        id: lblRunning
                        x: parent.x + 48
                        y: 10
                        text: qsTr("0 tasks runinng")
                        font.bold: true
                        font.pointSize: 12
                    }
                }

                Image {
                    id: imgInQueue
                    x: 10
                    y: 178
                    width: 48
                    height: 48
                    source: "Graphics/queue.png"
                    Text {
                        id: lblInQueue
                        x: parent.x + 48
                        y: 10
                        text: qsTr("0 tasks in queue")
                        font.bold: true
                        font.pointSize: 12
                    }
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

                Image {
                    id: logo
                    source: "Graphics/pslogo.gif"
                    width: 200
                    height: 200
                    x: parent.width - 250
                }

                ProgressBar {
                    id: progressBar
                    x: 50
                    y: parent.height - 100
                    width: parent.width - 100
                    height: 32
                    minimumValue: 0.0
                    maximumValue: 100.0
                }

                TextInput {
                    id: textInput
                    x: 438
                    y: 280
                    width: 80
                    height: 20
                    text: qsTr("1")
                    font.pixelSize: 48
                }

                Image {
                    id: btnCustom
                    x: 500
                    y: 250
                    width: 48
                    height: 48
                    source: "Graphics/intruder.png"

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {

                            btnCustomSignal(textInput.text);

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
            id: dataTab
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
