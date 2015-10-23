function Component()
{
}

Component.prototype.createOperations = function()
{
    component.createOperations();
    if (systemInfo.productType === "windows")
    {
        console.log("Creating additional shortcuts");
        component.addOperation("CreateShortcut",
                               "@TargetDir@/MaintenanceTool.exe",
                               "@StartMenuDir@/" + qsTr("Maintenance", "Start menu entry") + ".lnk");
        component.addOperation("AppendFile",
                               "@StartMenuDir@\\" + qsTr("Project Page", "Start menu link") + ".url",
                               "[InternetShortcut]\nURL=https://github.com/mentaljam/MSUProj");
        component.addOperation("AppendFile",
                               "@StartMenuDir@\\" + qsTr("Issues and Suggestions", "Start menu link") + ".url",
                               "[InternetShortcut]\nURL=https://github.com/mentaljam/MSUProj/issues");
        component.addOperation("AppendFile",
                               "@StartMenuDir@\\" + qsTr("Robonuka Meteor-M N2 Site", "Start menu link") + ".url",
                               "[InternetShortcut]\nURL=http://meteor.robonuka.ru/");
    }
}
