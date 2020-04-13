/*
 * This file is part of ATSDB.
 *
 * ATSDB is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ATSDB is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with ATSDB.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef STRUCTUREDESCRIPTIONADSB_H_
#define STRUCTUREDESCRIPTIONADSB_H_

#include <cstddef>

#include "ADSB.h"

/**
 * @brief Description for ADS-D data struct t_Adsb
 *
 * Creates a temporary description, and adds flattened contents to itself
 */
class StructureDescriptionADSB : public StructureDescription
{
  public:
    StructureDescriptionADSB() : StructureDescription(DBO_ADS_B, "ADS_B", "RDL t_Adsb structure", 0)
    {
        create();
    }
    virtual ~StructureDescriptionADSB() {}

  protected:
    void create()
    {
        StructureDescription* rdl_description =
            new StructureDescription(DBO_ADS_B, "", "rdl t_Adsb structure", 0);

        //  t_Data_Source_Identifier data_source_identifier;
        //                   /* Data source identifier */
        size_t offset = offsetof(t_Adsb, data_source_identifier);
        StructureDescription* data_source_identifier = rdl_description->addStructureDescription(
            "data_source_identifier", "Data source identifier", offset);
        data_source_identifier->addPresentStructureVariable(
            "present", SE_TYPE_BOOL, 1, "Data source identifier present",
            offset + offsetof(t_Data_Source_Identifier, present));
        data_source_identifier->addStructureVariable(
            "supplemented", SE_TYPE_BOOL, 1, "Data source iData source identifier supplemented",
            offset + offsetof(t_Data_Source_Identifier, supplemented));
        data_source_identifier->addStructureVariable(
            "value", SE_TYPE_USMALLINT, 1, "Data source identifier (SAC/SIC)",
            offset + offsetof(t_Data_Source_Identifier, value));

        // t_Time_of_Day detection_time;
        /* Measured detection time */
        offset = offsetof(t_Adsb, detection_time);
        StructureDescription* detection_time = rdl_description->addStructureDescription(
            "detection_time", "Measured detection time", offset);
        detection_time->addPresentStructureVariable("present", SE_TYPE_BOOL, 1,
                                                    "Time of day present",
                                                    offset + offsetof(t_Time_of_Day, present));
        detection_time->addStructureVariable("value", SE_TYPE_DOUBLE, 1, "Time of day; seconds",
                                             offset + offsetof(t_Time_of_Day, value));

        // t_Detection_Type detection_type;
        /* Detection type */
        offset = offsetof(t_Adsb, detection_type);
        StructureDescription* detection_type =
            rdl_description->addStructureDescription("detection_type", "Detection type", offset);
        detection_type->addStructureVariable(
            "from_fixed_field_transponder", SE_TYPE_UTINYINT, 1,
            "Target report originating from Fixed Field Transponder (FFT) which is also called a "
            "\"parrot\" or RABM (range and bearing monitor) sometimes",
            offset + offsetof(t_Detection_Type, from_fixed_field_transponder));
        detection_type->addStructureVariable(
            "is_ctr_target", SE_TYPE_UTINYINT, 1,
            "Is a control target (some type of RTQC as used on Russian radar lines)",
            offset + offsetof(t_Detection_Type, from_fixed_field_transponder));
        detection_type->addStructureVariable("is_radar_track", SE_TYPE_UTINYINT, 1,
                                             "Is a radar track (not a radar plot)",
                                             offset + offsetof(t_Detection_Type, is_radar_track));
        detection_type->addStructureVariable(
            "is_raw_plot", SE_TYPE_UTINYINT, 1,
            "Is a so-called \"raw\" plot, i. e. rather unprocessed target from an early stage",
            offset + offsetof(t_Detection_Type, is_raw_plot));
        detection_type->addStructureVariable("is_rbs_target", SE_TYPE_UTINYINT, 1,
                                             "Is a so-called ATCRBS target report, i. e. "
                                             "Western-style report on Russian radar lines",
                                             offset + offsetof(t_Detection_Type, is_rbs_target));
        detection_type->addStructureVariable("is_uvd_target", SE_TYPE_UTINYINT, 1,
                                             "Is a VRL UVD target report",
                                             offset + offsetof(t_Detection_Type, is_uvd_target));
        detection_type->addStructureVariable("mode_s_all_call", SE_TYPE_UTINYINT, 1,
                                             "Response to an SSR mode S all-call",
                                             offset + offsetof(t_Detection_Type, mode_s_all_call));
        detection_type->addStructureVariable("mode_s_roll_call", SE_TYPE_UTINYINT, 1,
                                             "Response to an SSR mode S roll-call",
                                             offset + offsetof(t_Detection_Type, mode_s_roll_call));
        detection_type->addStructureVariable("monopulse", SE_TYPE_UTINYINT, 1,
                                             "Monopulse detection (not sliding window)",
                                             offset + offsetof(t_Detection_Type, monopulse));
        detection_type->addStructureVariable("passive_reply", SE_TYPE_BOOL, 1, "Passive reply",
                                             offset + offsetof(t_Detection_Type, passive_reply));
        detection_type->addPresentStructureVariable("present", SE_TYPE_BOOL, 1,
                                                    "Detection type present",
                                                    offset + offsetof(t_Detection_Type, present));
        detection_type->addStructureVariable(
            "reported_from_ads", SE_TYPE_UTINYINT, 1, "Reported from an ADS-B groundstation",
            offset + offsetof(t_Detection_Type, reported_from_ads));
        detection_type->addStructureVariable(
            "reported_from_mds", SE_TYPE_UTINYINT, 1, "Reported from a Mode S sensor (tracker)",
            offset + offsetof(t_Detection_Type, reported_from_mds));
        detection_type->addStructureVariable(
            "reported_from_mlt", SE_TYPE_UTINYINT, 1, "Reported from a multilateration system",
            offset + offsetof(t_Detection_Type, reported_from_mlt));
        detection_type->addStructureVariable(
            "reported_from_psr", SE_TYPE_UTINYINT, 1, "Reported from a Primary Surveillance Radar",
            offset + offsetof(t_Detection_Type, reported_from_psr));
        detection_type->addStructureVariable(
            "reported_from_ssr", SE_TYPE_UTINYINT, 1,
            "Reported from a Secondary Surveillance Radar",
            offset + offsetof(t_Detection_Type, reported_from_ssr));
        detection_type->addStructureVariable("sector_crossing", SE_TYPE_BOOL, 1, "Sector crossing",
                                             offset + offsetof(t_Detection_Type, sector_crossing));
        detection_type->addStructureVariable("simulated", SE_TYPE_UTINYINT, 1,
                                             "Simulated (not actual) target",
                                             offset + offsetof(t_Detection_Type, simulated));
        detection_type->addStructureVariable("test_target", SE_TYPE_UTINYINT, 1,
                                             "(Internal or external) test target",
                                             offset + offsetof(t_Detection_Type, test_target));

        //  t_Figure_of_Merit figure_of_merit;
        //                   /* Figure of merit (position accuracy indication) */
        offset = offsetof(t_Adsb, figure_of_merit);
        StructureDescription* figure_of_merit = rdl_description->addStructureDescription(
            "figure_of_merit", "Figure of merit (position accuracy indication)", offset);
        figure_of_merit->addPresentStructureVariable("present", SE_TYPE_BOOL, 1,
                                                     "Figure of merit present",
                                                     offset + offsetof(t_Figure_of_Merit, present));
        figure_of_merit->addStructureVariable("value", SE_TYPE_UTINYINT, 1, "Figure of merit",
                                              offset + offsetof(t_Figure_of_Merit, value));

        // t_Frame_Date frame_date;
        /* Frame date */
        offset = offsetof(t_Adsb, frame_date);
        StructureDescription* frame_date =
            rdl_description->addStructureDescription("frame_date", "Frame date", offset);
        frame_date->addPresentStructureVariable("present", SE_TYPE_BOOL, 1, "Frame date present",
                                                offset + offsetof(t_Frame_Date, present));
        offset = offset + offsetof(t_Frame_Date, value);
        StructureDescription* value =
            frame_date->addStructureDescription("value", "Frame date", offset);
        value->addStructureVariable("year", SE_TYPE_INT, 1, "Frame date year",
                                    offset + offsetof(t_Date, year));
        value->addStructureVariable("month", SE_TYPE_INT, 1, "Frame date month",
                                    offset + offsetof(t_Date, month));
        value->addStructureVariable("day", SE_TYPE_INT, 1, "Frame date day",
                                    offset + offsetof(t_Date, day));

        // t_Frame_Time frame_time;
        /* Frame time */
        offset = offsetof(t_Adsb, frame_time);
        StructureDescription* frame_time =
            rdl_description->addStructureDescription("frame_time", "Frame time", offset);
        frame_time->addPresentStructureVariable("present", SE_TYPE_BOOL, 1, "Frame time present",
                                                offset + offsetof(t_Frame_Time, present));
        frame_time->addStructureVariable("value", SE_TYPE_DOUBLE, 1, "Frame time (of day); seconds",
                                         offset + offsetof(t_Frame_Time, value));

        //  t_Bool ground_bit_set;
        //                   /* ADS-B target reported to be on ground */
        rdl_description->addStructureVariable("ground_bit_set", SE_TYPE_BOOL, 1,
                                              "ADS-B target reported to be on ground",
                                              offsetof(t_Adsb, ground_bit_set));

        //  t_Ground_Vector ground_vector;
        //                   /* Ground speed and heading */
        offset = offsetof(t_Adsb, ground_vector);
        StructureDescription* ground_vector = rdl_description->addStructureDescription(
            "ground_vector", "Ground speed and heading", offset);
        ground_vector->addPresentStructureVariable("present", SE_TYPE_BOOL, 1,
                                                   "Ground vector present",
                                                   offset + offsetof(t_Ground_Vector, present));
        ground_vector->addStructureVariable("value_gsp", SE_TYPE_DOUBLE, 1,
                                            "Ground speed; metres/second",
                                            offset + offsetof(t_Ground_Vector, value_gsp));
        ground_vector->addStructureVariable("value_hdg", SE_TYPE_DOUBLE, 1,
                                            "Ground speed; Heading; radians",
                                            offset + offsetof(t_Ground_Vector, value_hdg));

        //  t_Mode_Info mode_3a_info;
        //                   /* SSR mode 3/A information */
        offset = offsetof(t_Adsb, mode_3a_info);
        StructureDescription* mode_3a_info = rdl_description->addStructureDescription(
            "mode_3a_info", "SSR mode 3/A information", offset);
        mode_3a_info->addStructureVariable("code", SE_TYPE_USMALLINT, 1,
                                           "SSR mode 1, 2, or 3/A code",
                                           offset + offsetof(t_Mode_Info, code));
        mode_3a_info->addStructureVariable(
            "code_confidence", SE_TYPE_USMALLINT, 1,
            "SSR mode 1, 2, or 3/A code confidence  Not inverted, i. e. 0=high confidence",
            offset + offsetof(t_Mode_Info, code_confidence));
        mode_3a_info->addStructureVariable("code_confidence_present", SE_TYPE_BOOL, 1,
                                           "SSR mode 1, 2, or 3/A code confidence present",
                                           offset + offsetof(t_Mode_Info, code_confidence_present));
        mode_3a_info->addStructureVariable("code_garbled", SE_TYPE_UTINYINT, 1,
                                           "SSR mode 1, 2, or 3/A code garbled",
                                           offset + offsetof(t_Mode_Info, code_garbled));
        mode_3a_info->addStructureVariable("code_invalid", SE_TYPE_UTINYINT, 1,
                                           "SSR mode 1, 2, or 3/A code invalid",
                                           offset + offsetof(t_Mode_Info, code_invalid));
        mode_3a_info->addStructureVariable("code_smoothed", SE_TYPE_UTINYINT, 1,
                                           "SSR mode 1, 2, or 3/A code smoothed",
                                           offset + offsetof(t_Mode_Info, code_smoothed));
        mode_3a_info->addPresentStructureVariable("present", SE_TYPE_BOOL, 1,
                                                  "SSR mode 1, 2, or 3/A information present",
                                                  offset + offsetof(t_Mode_Info, present));
        mode_3a_info->addStructureVariable("replies", SE_TYPE_USMALLINT, 1,
                                           "Number of SSR mode 1, 2, or 3/A replies",
                                           offset + offsetof(t_Mode_Info, replies));

        //  t_Mode_C_Height mode_c_height;
        //                   /* SSR mode C height */
        offset = offsetof(t_Adsb, mode_c_height);
        StructureDescription* mode_c_height =
            rdl_description->addStructureDescription("mode_c_height", "SSR mode C height", offset);
        mode_c_height->addStructureVariable("garbled", SE_TYPE_UTINYINT, 1,
                                            "Garbled SSR mode C height",
                                            offset + offsetof(t_Mode_C_Height, garbled));
        mode_c_height->addStructureVariable(
            "height_in_error", SE_TYPE_UTINYINT, 1,
            "SSR mode C height is not a valid altitude, i.e. it is out of (regular) bounds",
            offset + offsetof(t_Mode_C_Height, height_in_error));
        mode_c_height->addStructureVariable("in_25_feet", SE_TYPE_UTINYINT, 1,
                                            "SSR mode C height in 25 feet resolution",
                                            offset + offsetof(t_Mode_C_Height, in_25_feet));
        mode_c_height->addStructureVariable("invalid", SE_TYPE_UTINYINT, 1,
                                            "Invalid SSR mode C height",
                                            offset + offsetof(t_Mode_C_Height, invalid));
        mode_c_height->addPresentStructureVariable("present", SE_TYPE_BOOL, 1,
                                                   "SSR mode C information present",
                                                   offset + offsetof(t_Mode_C_Height, present));
        mode_c_height->addStructureVariable("value", SE_TYPE_DOUBLE, 1, "SSR mode C height; metres",
                                            offset + offsetof(t_Mode_C_Height, value));
        mode_c_height->addStructureVariable("value_in_feet", SE_TYPE_INT, 1,
                                            "SSR mode C height; feet",
                                            offset + offsetof(t_Mode_C_Height, value_in_feet));

        //  t_Sensor_Number sensor_number;
        //                   /* Sensor number */
        offset = offsetof(t_Adsb, sensor_number);
        StructureDescription* sensor_number =
            rdl_description->addStructureDescription("sensor_number", "Sensor number", offset);
        sensor_number->addPresentStructureVariable("present", SE_TYPE_BOOL, 1,
                                                   "Sensor number present",
                                                   offset + offsetof(t_Sensor_Number, present));
        sensor_number->addStructureVariable(
            "value", SE_TYPE_INT, 1, "Sensor number  Must be from range 1 ... MAX_NUMBER_OF_RADARS",
            offset + offsetof(t_Sensor_Number, value));

        //  t_Bool special_position_indication;
        //                   /* Special Position Indication flag */
        rdl_description->addStructureVariable("special_position_indication", SE_TYPE_BOOL, 1,
                                              "Special Position Indication flag",
                                              offsetof(t_Adsb, special_position_indication));

        //  t_Aircraft_Address target_address;
        //                   /* Target address */
        offset = offsetof(t_Adsb, target_address);
        StructureDescription* target_address =
            rdl_description->addStructureDescription("target_address", "Target address", offset);
        target_address->addPresentStructureVariable("present", SE_TYPE_BOOL, 1,
                                                    "Aircraft address present",
                                                    offset + offsetof(t_Aircraft_Address, present));
        target_address->addStructureVariable("value", SE_TYPE_UINT, 1, "Aircraft address",
                                             offset + offsetof(t_Aircraft_Address, value));

        //  t_Target_Address_Type target_address_type;
        //                   /* Target address type */
        offset = offsetof(t_Adsb, target_address_type);
        StructureDescription* target_address_type = rdl_description->addStructureDescription(
            "target_address_type", "Target address type", offset);
        target_address_type->addPresentStructureVariable(
            "present", SE_TYPE_BOOL, 1, "Target address type present",
            offset + offsetof(t_Target_Address_Type, present));
        target_address_type->addStructureVariable("value", SE_TYPE_UTINYINT, 1,
                                                  "Target address enumeration type",
                                                  offset + offsetof(t_Target_Address_Type, value));

        //  t_Aircraft_Identification target_identification;
        //                   /* Target identification */
        offset = offsetof(t_Adsb, target_identification);
        StructureDescription* target_identification = rdl_description->addStructureDescription(
            "target_identification", "Target identification", offset);
        target_identification->addPresentStructureVariable(
            "present", SE_TYPE_BOOL, 1, "Aircraft identification present",
            offset + offsetof(t_Aircraft_Identification, present));
        target_identification->addStructureVariable(
            "sti_present", SE_TYPE_BOOL, 1, "Source of target identification present",
            offset + offsetof(t_Aircraft_Identification, sti_present));
        target_identification->addStructureVariable(
            "value_idt", SE_TYPE_VARCHAR_ARRAY, M_AIRCRAFT_IDENTIFICATION_LENGTH + 1,
            "Aircraft identification", offset + offsetof(t_Aircraft_Identification, value_idt));
        target_identification->addStructureVariable(
            "value_sti", SE_TYPE_UTINYINT, 1, "Source of target identification",
            offset + offsetof(t_Aircraft_Identification, value_sti));

        //  t_WGS84_Position wgs84_position;
        //                   /* WGS-84 position */
        offset = offsetof(t_Adsb, wgs84_position);
        StructureDescription* wgs84_position =
            rdl_description->addStructureDescription("wgs84_position", "WGS-84 position", offset);
        wgs84_position->addPresentStructureVariable("present", SE_TYPE_BOOL, 1,
                                                    "WGS-84 position present",
                                                    offset + offsetof(t_WGS84_Position, present));
        wgs84_position->addStructureVariable("value_lat", SE_TYPE_DOUBLE, 1,
                                             "WGS-84 latitude; radians",
                                             offset + offsetof(t_WGS84_Position, value_lat));
        wgs84_position->addStructureVariable("value_lon", SE_TYPE_DOUBLE, 1,
                                             "WGS-84 longitude; radians",
                                             offset + offsetof(t_WGS84_Position, value_lon));

        rdl_description->addToFlatStructureDescription(this, "");
        delete rdl_description;
    }
};

#endif /* STRUCTUREDESCRIPTIONADSB_H_ */
