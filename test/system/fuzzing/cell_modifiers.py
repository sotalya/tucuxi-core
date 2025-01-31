"""
Set of cell modifiers that are used to fuzz the CDSS CLI (by altering the values
in the XML file.
"""


class E(Exception):
    pass


class CellModifier:
    """
    Cell modifiers that alter XML values.
    """
    @staticmethod
    def modify_num_to_0(cell_tag, cell_text):
        float(cell_text)
        return 0

    @staticmethod
    def modify_num_to_negative(cell_tag, cell_text):
        value = float(cell_text)
        return -value

    @staticmethod
    def multiply_values(cell_tag, cell_text):
        value = float(cell_text)
        return value * 1000

    @staticmethod
    def modify_units(cell_tag, cell_text):
        if cell_tag == "unit":
            if cell_text == "kg":
                return "kg/l"
            if cell_text == "kg/l":
                return "kg"
            if cell_text == "mg":
                return "mg/l"
            if cell_text == "mg/l":
                return "mg"
            if cell_text == "ug":
                return "ug/l"
            if cell_text == "ug/l":
                return "ug"
        raise E()

    @staticmethod
    def inject_non_numeric_values(cell_tag, cell_text):
        return "NaN"

    @staticmethod
    def inject_extreme_numeric_values(cell_tag, cell_text):
        return 1e308

    @staticmethod
    def inject_malformed_numeric_values(cell_tag, cell_text):
        return "12..34"

    @staticmethod
    def inject_invalid_dates(cell_tag, cell_text):
        return "31/02/2023"

    @staticmethod
    def inject_ambiguous_or_invalid_dates(cell_tag, cell_text):
        return "9999-99-99"

    @staticmethod
    def inject_non_standard_timezones(cell_tag, cell_text):
        return "2023-05-12T00:00:00+25:00"

    @staticmethod
    def inject_invalid_boolean(cell_tag, cell_text):
        return "true"

    @staticmethod
    def inject_long_text(cell_tag, cell_text):
        return "A" * 10000

    @staticmethod
    def inject_special_characters(cell_tag, cell_text):
        return "<>&'\"\0\n"

    @staticmethod
    def inject_unicode_characters(cell_tag, cell_text):
        return "💉🌐😀"

    @staticmethod
    def inject_invalid_units(cell_tag, cell_text):
        return "liters/kg"

    @staticmethod
    def inject_empty_units(cell_tag, cell_text):
        return ""
