/*
 * Tucuxi - Tucuxi-core library and command line tool.
 * This code allows to perform prediction of drug concentration in blood
 * and to propose dosage adaptations.
 * It has been developed by HEIG-VD, in close collaboration with CHUV.
 * Copyright (C) 2023 HEIG-VD, maintained by Yann Thoma  <yann.thoma@heig-vd.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <gtest/gtest.h>

#include "tucucommon/translatablestring.h"

TEST(Common_TestTranslatableString, Basic)
{
    Tucuxi::Common::TranslatableString str("Hello world", "en");
    EXPECT_EQ(str.getString(), "Hello world");
}

TEST(Common_TestTranslatableString, MultipleLanguages)
{
    Tucuxi::Common::TranslatableString str("Hello world", "en");
    str.setString("Bonjour le monde", "fr");
    EXPECT_EQ(str.getString(), "Hello world");
    EXPECT_EQ(str.getString("fr"), "Bonjour le monde");
}

TEST(Common_TestTranslatableString, MissingLanguage)
{
    Tucuxi::Common::TranslatableString str("Hello world", "en");
    EXPECT_EQ(str.getString("fr"), "");
}

TEST(Common_TestTranslatableString, GetLanguages)
{
    Tucuxi::Common::TranslatableString str("Hello world", "en");
    str.setString("Bonjour le monde", "fr");
    std::vector<std::string> languages = str.getLanguages();
    EXPECT_EQ(languages.size(), 2);
    EXPECT_TRUE(std::find(languages.begin(), languages.end(), "en") != languages.end());
    EXPECT_TRUE(std::find(languages.begin(), languages.end(), "fr") != languages.end());
}


TEST(Common_TestTranslatableString, ChangeDefaultLanguage)
{
    Tucuxi::Common::TranslatableString::setDefaultLanguage("fr");

    Tucuxi::Common::TranslatableString str("Bonjour le monde", "fr");
    EXPECT_EQ(str.getString(), "Bonjour le monde");
}