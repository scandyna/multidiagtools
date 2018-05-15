
import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Window 2.2

ApplicationWindow
{
  visible: true
  title: qsTr("Hello")
  width: 640
  height: 480

  menuBar: MenuBar
  {
    Menu
    {
      title: qsTr("File")
      MenuItem
      {
        text: qsTr("&Open")
        onTriggered: console.log("Open")
      }
    }
  }

  Component
  {
    id: contactDelegate
    Item
    {
      width: 180
      height: 40
      Column
      {
        Text { text: '<b>Name:</b>' + name }
        Text { text: '<b>Number:</b>' + number }
      }
    }
  }

  ListView
  {
    width: 180
    height: 200

    anchors.fill: parent
    model: ContactModel {}
    delegate: contactDelegate
    highlight: Rectangle
    {
      color: "lightsteelblue"
      radius: 5
    }
  }
}
