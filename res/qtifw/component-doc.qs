function Component()
{
}

Component.prototype.createOperations = function()
{
    if (installer.value("os") === "win")
    {
        console.log("Creating additional shortcuts");
        component.addOperation("CreateShortcut",
                               "@TargetDir@/MaintenanceTool.exe",
                               "@StartMenuDir@/Maintenance.lnk");
        component.addOperation("AppendFile",
                               "@StartMenuDir@\\Project Page.url",
                               "[InternetShortcut]\nURL=https://github.com/mentaljam/MSUProj");
        component.addOperation("AppendFile",
                               "@StartMenuDir@\\Issues and Suggestions.url",
                               "[InternetShortcut]\nURL=https://github.com/mentaljam/MSUProj/issues");
        component.addOperation("AppendFile",
                               "@StartMenuDir@\\Robonuka Meteor-M N2 Site.url",
                               "[InternetShortcut]\nURL=http://meteor.robonuka.ru/");
    }
}
