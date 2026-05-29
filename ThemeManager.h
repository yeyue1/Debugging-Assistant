#pragma once

#include <QString>

class ThemeManager
{
public:
    static ThemeManager& instance();

    QString darkTheme() const;
    QString lightTheme() const;

    void applyDarkTheme();
    void applyLightTheme();

private:
    ThemeManager() = default;
};
