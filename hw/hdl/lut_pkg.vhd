-- Copyright (C) 2014 Roland Dobai
--
-- This file is part of ZyEHW.
--
-- ZyEHW is free software: you can redistribute it and/or modify it under the
-- terms of the GNU General Public License as published by the Free Software
-- Foundation, either version 3 of the License, or (at your option) any later
-- version.
--
-- ZyEHW is distributed in the hope that it will be useful, but WITHOUT ANY
-- WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
-- FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
-- details.
--
-- You should have received a copy of the GNU General Public License along
-- with ZyEHW. If not, see <http://www.gnu.org/licenses/>.

library ieee;
use ieee.std_logic_1164.all;

use work.zyehw_pkg.all;

package lut_pkg is
    subtype lut_t is bit_vector(63 downto 0);
    type pe_lut_t is array (cgp_t'reverse_range) of lut_t;
    type pe_lut_arr_t is array (0 to (rows-1), 0 to (columns-1)) of pe_lut_t;

    -- these are random numbers because otherwise the synthesizer would
    -- remove some of the PE items
    constant dummy_pe_lut_arr: pe_lut_arr_t:= (
        (
            (
            X"000000005851F42D",
            X"40B18CCF4BB5F646",
            X"4703312930705B04",
            X"20FD5DB41A8B7F78",
            X"502959D82B894868",
            X"6C0356A708CDB7FF",
            X"3477D43F70A3A52B",
            X"28E4BAF17D8341FC"
            ),
            (
            X"0AE16FD9742D2F7A",
            X"0D1F079676035E09",
            X"40F7702C6FA72CA5",
            X"2AA8415758A0DF74",
            X"474A03642E533CC4",
            X"04185FAF6DE3B115",
            X"0CAB86287043BFA4",
            X"398150E937521657"
            ),
            (
            X"4D9A2FDB76C3625E",
            X"55075020329B54FB",
            X"381FC0385378D5A8",
            X"76E5004E797D87A6",
            X"2756C7144C598D93",
            X"643BD7252B449456",
            X"671286247899E644",
            X"088232DF6DB50559"
            ),
            (
            X"7D157D8D71DFED43",
            X"45CB977B6EA3BBBE",
            X"3A984B90666D9716",
            X"6FD595896931589D",
            X"17C5E07E0B0ABB77",
            X"51AF16C61DEE132F",
            X"3B3A772600F32298",
            X"5148068835EB8DF5"
            ),
            (
            X"7A1179B41AE53A30",
            X"7C418DD42170B934",
            X"6DB4489D4A691C3A",
            X"5A89680D249AD60F",
            X"28BAE6BD526D022F",
            X"6DE3EDA1559E2FF7",
            X"38B9210F072790D9",
            X"638391684C1EE2F4"
            ),
            (
            X"0D1102D92657C93C",
            X"79B12BA855365135",
            X"794F06E57091758F",
            X"59B5FE2A302059CD",
            X"4E8694C26E7B6440",
            X"116E0FC138479ACF",
            X"23252C5217E96171",
            X"40C9FE6A49067A72"
            ),
            (
            X"561F3B294FD30023",
            X"07D89DB046755A5B",
            X"482FA51B44AEB1A3",
            X"0D57872A4CBD7E82",
            X"235E94882BD25B44",
            X"5EC1CC197B5F2B0E",
            X"27DE5630302D71F2",
            X"47A3556B3C7F6AA3"
            ),
            (
            X"7C55D8AE512B300F",
            X"2D5E8A842FBE7DCD",
            X"2B847F490057B6D6",
            X"5DF6E2834B941D42",
            X"4CC54C177044FFE7",
            X"274A1FA301B18039",
            X"7FDF0C93446D8073",
            X"6A50ABC676E04EE6"
            )
        ),
        (
            (
            X"00611EF139B7048A",
            X"5CFAA8FE26FAAC21",
            X"77E1241B0E63C119",
            X"29AE8B7E0F90F8E4",
            X"755B6B7570C23C8E",
            X"72F7FCC6205C9587",
            X"43AF20857ABEBC58",
            X"1F1B54DD340C3024"
            ),
            (
            X"7B7620E675214E9D",
            X"5AEC5E5D0CAD3244",
            X"587F60406E6AA68B",
            X"1FD3905A22C4F223",
            X"7C06BA5E0CF05DA9",
            X"36745DEF7325D2EC",
            X"775CFE3F0E223791",
            X"7502199C312FAB7A"
            ),
            (
            X"6559AA410BBADF17",
            X"2A16DB1800985972",
            X"75C9AEEB4EA0C606",
            X"1A637BF20DD45F91",
            X"227B64EE121412B8",
            X"42967A6C4BB1E6BE",
            X"1C2099D9500F7B34",
            X"7E0D3DF744AC5234"
            ),
            (
            X"142FEF23690AE306",
            X"6849ED7A25D23D33",
            X"426B1E70040E4297",
            X"2E03D8D7274EE452",
            X"0346C6125E36B2F5",
            X"74A7347E742072D2",
            X"20F8FF943D044295",
            X"0C75DB6622FEFE06"
            ),
            (
            X"63D6E1236C52F559",
            X"13DE80F246AC426F",
            X"6A4677FA1203EEE7",
            X"767BB4EC22E2E0A9",
            X"467C6EE63E0E58AB",
            X"665297AA003A5839",
            X"773336BF05DF1FBA",
            X"0D20D75F5F6694A5"
            ),
            (
            X"2C86C8B15DE2D8AE",
            X"7FDD81242AD45A81",
            X"14CC39617DC30B8A",
            X"521B4C7811305745",
            X"4817E9E417E9263C",
            X"09EFFE474CBE2C78",
            X"7756E63935F58C47",
            X"14B17CC737B9759D"
            ),
            (
            X"455E81D9476E8C4A",
            X"7B6797AF3BCBFD61",
            X"0C2CB017268FBB3B",
            X"4681C3BD0C7FE26A",
            X"080778F52FCBC7D0",
            X"000135441E63235B",
            X"4272E9912930A9CD",
            X"78AB7F5F24308D75"
            ),
            (
            X"1B5A7B5F49202F41",
            X"2103FBB8713585D5",
            X"00D9443830D8D007",
            X"289C95FE48D313D8",
            X"25A5CD5A391204B1",
            X"4170B52F1DE77846",
            X"7F31C5BC4206B48E",
            X"07BFF03541F7C92A"
            )
        ),
        (
            (
            X"43FF573804D5E4CC",
            X"1EDC4A2A7B8D5AAF",
            X"28ADC2AF30C25462",
            X"5498210F72A45A86",
            X"332A78752C2EF355",
            X"332EC7743FEC221C",
            X"557BAC7028E1EBA3",
            X"1F746A094E0C64E3"
            ),
            (
            X"40BACB5F48E2ACBE",
            X"4DBB1DB34CA91014",
            X"7EF0357A1132F327",
            X"44C47458033D4927",
            X"354A946B1FA92310",
            X"17552ED80A7C5CB1",
            X"7AB97D104170719A",
            X"75D77CAE515A5105"
            ),
            (
            X"40A722086FBFE338",
            X"55E2DD847DCB6BD6",
            X"0A1B990A24872187",
            X"242D4A553FCAD87A",
            X"03E4E3A35E1DCE75",
            X"51F0AA352AEC52BF",
            X"1809F23E5A736871",
            X"6FE91E6C2322A884"
            ),
            (
            X"6DF778271E63DA77",
            X"2A81F9C13740F7D1",
            X"621F7DC2473FADE0",
            X"0ECFD39645480065",
            X"562666204BF4E964",
            X"57F7686D7936F374",
            X"6EE7EB0755EE4C0B",
            X"44C1AA5D4F7DF387"
            ),
            (
            X"16DC99605368C1DE",
            X"0AA98CB7314D8D5D",
            X"5964648B6F641684",
            X"09F4BA361AF7C2E0",
            X"6B1623BB127E9DBF",
            X"19CB4497662FBE85",
            X"7BDE7EA20E124E0A",
            X"7655B2C10945B15A"
            ),
            (
            X"232C184841D5CD1C",
            X"353B17C935B804C5",
            X"407A658958E3CB69",
            X"574084D2137883AA",
            X"52E8AA321F7189CF",
            X"0BEE617173BA11C8",
            X"0DF549D77AEC3614",
            X"4EF8AE5164D964A2"
            ),
            (
            X"247D020667334E8D",
            X"5041D31D4075F3CD",
            X"5BDD6FED24043DC9",
            X"18883FFA157B47C9",
            X"39DD7E18032FD357",
            X"54487B0545C44367",
            X"787D24FD01B60989",
            X"12744794780C1CEA"
            ),
            (
            X"41C74C53599937BD",
            X"679BC6FF08DE2644",
            X"68B031DC1ADC60A5",
            X"4564152004DA37D5",
            X"16BA7DA341D7E24C",
            X"5483058E24019394",
            X"47D331934CB87DA1",
            X"66ADD5246F103874"
            )
        ),
        (
            (
            X"25DCDCCD1B9E331A",
            X"16B6A604675D3BA3",
            X"14B4777F5A00FD41",
            X"31D535FF3F2A8012",
            X"213AF44F6EF45642",
            X"0699F098734AC0CB",
            X"548DEE737DE2F6F6",
            X"1E6741F97A6FD954"
            ),
            (
            X"4D4CC8B3057449D4",
            X"29741EEC41EC94B5",
            X"331D901E32D7FE82",
            X"5320437F3FF66956",
            X"53F9DE63159D897B",
            X"23C6A5604E29C9A7",
            X"4F55C1AA6AAB997F",
            X"04AC6CAC7B0948E1"
            ),
            (
            X"706B2BF03016C8E4",
            X"7D091D3877807C4D",
            X"571F0866378B963A",
            X"0939171E548D45B7",
            X"55809B502B0DB4AA",
            X"1362976702622738",
            X"066826E04163ABEE",
            X"04F1C4B64A9F4466"
            ),
            (
            X"2D51B68C7533D847",
            X"66DB2F8334473607",
            X"724FC8C36548DF64",
            X"73C895C319C78C0C",
            X"51350DEA44078660",
            X"63467E4861438AEE",
            X"507271666AF75183",
            X"0ECCACAB3DD5EE27"
            ),
            (
            X"5FB6CF733072F75C",
            X"20FF0A9439E75E18",
            X"3C1F66D73D586049",
            X"3DF0F31F1B3E3D30",
            X"66E3E98129FDC121",
            X"49830FCD0E41EB0D",
            X"530ED0D91C0A5D46",
            X"5881E8714B6DDDB1"
            ),
            (
            X"1D72EB9A28E05172",
            X"6E0AE02A03F0C525",
            X"1CB139113CB18F8C",
            X"4F808C811F456E1A",
            X"4295AAD467A9332D",
            X"2417D13456AA68AA",
            X"61BC0768418EC1AC",
            X"256A1E7670378A72"
            ),
            (
            X"727578810F9B9D87",
            X"0C6F008B5567B32B",
            X"34767B5A6FED3F2F",
            X"713F4226417A91BE",
            X"023E7BDE21A91AEC",
            X"528456BF2A595A38",
            X"10A600AD334BDFB0",
            X"706032DF3B920BA3"
            ),
            (
            X"11C4790D7F6FFC29",
            X"6876FDC834B73C6F",
            X"7ED693DF4AED9078",
            X"286DF60642E7DDB8",
            X"27AB01843AE8CB0A",
            X"5AB80872141285A2",
            X"6903382A4658B84C",
            X"4B1103AE3950E877"
            )
        )
    );
end;

package body lut_pkg is
end package body;
