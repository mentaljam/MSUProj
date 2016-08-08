function Component()
{
}

Component.prototype.createOperations = function()
{
    component.createOperations();
    if (systemInfo.productType === "windows")
    {
        var key;
        var redist;
        if(systemInfo.currentCpuArchitecture === "x86_64")
        {
            key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Wow6432Node\\Microsoft\\VisualStudio\\11.0\\VC\\Runtimes\\x86\\Installed";
            redist = "vcredist_x64.exe";
        }
        else
        {
            key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\11.0\\VC\\Runtimes\\x86\\Installed";
            redist = "vcredist_x86.exe";
        }
        if (installer.value(key) != 1)
        {
            component.addElevatedOperation("Execute", "@TargetDir@\\" + redist, "/norestart", "/q");
        }
    }
}
