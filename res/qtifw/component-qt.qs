function Component()
{
}

Component.prototype.createOperations = function()
{
    component.createOperations();
    if (systemInfo.productType === "windows")
    {
        component.addOperation("CreateShortcut",
                               "@TargetDir@/msuproj-qt.exe",
                               "@StartMenuDir@/MSUProj-Qt.lnk");
        component.addOperation("CreateShortcut",
                               "@TargetDir@/msuproj-qt.exe",
                               "@DesktopDir@/MSUProj-Qt.lnk");
    }
}
