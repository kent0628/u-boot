/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Defines data structures and APIs that allow an OS to interact with UEFI
 * firmware to query information about the device
 *
 * This file refers the following TCG specification.
 *  - TCG PC Client Platform Firmware Profile Specification
 *    https://trustedcomputinggroup.org/resource/pc-client-specific-platform-firmware-profile-specification/
 *
 *  - TCG EFI Protocol Specification
 *    https://trustedcomputinggroup.org/resource/tcg-efi-protocol-specification/
 *
 * Copyright (c) 2020, Linaro Limited
 */

#if !defined _EFI_TCG2_PROTOCOL_H_
#define _EFI_TCG2_PROTOCOL_H_

#include <efi_api.h>
#include <tpm-v2.h>

#define EFI_TCG2_PROTOCOL_GUID \
	EFI_GUID(0x607f766c, 0x7455, 0x42be, 0x93, \
		 0x0b, 0xe4, 0xd7, 0x6d, 0xb2, 0x72, 0x0f)

/* TPMV2 only */
#define TCG2_EVENT_LOG_FORMAT_TCG_2 0x00000002
#define EFI_TCG2_EXTEND_ONLY 0x0000000000000001
#define PE_COFF_IMAGE 0x0000000000000010

#define EFI_TCG2_MAX_PCR_INDEX 23

/* Algorithm Registry */
#define EFI_TCG2_BOOT_HASH_ALG_SHA1    0x00000001
#define EFI_TCG2_BOOT_HASH_ALG_SHA256  0x00000002
#define EFI_TCG2_BOOT_HASH_ALG_SHA384  0x00000004
#define EFI_TCG2_BOOT_HASH_ALG_SHA512  0x00000008
#define EFI_TCG2_BOOT_HASH_ALG_SM3_256 0x00000010

#define EFI_TCG2_FINAL_EVENTS_TABLE_VERSION 1

#define TPM2_EVENT_LOG_SIZE CONFIG_EFI_TCG2_PROTOCOL_EVENTLOG_SIZE

typedef u32 efi_tcg_event_log_bitmap;
typedef u32 efi_tcg_event_log_format;
typedef u32 efi_tcg_event_algorithm_bitmap;

/**
 * struct tdEFI_TCG2_VERSION - structure of EFI TCG2 version
 * @major:	major version
 * @minor:	minor version
 */
struct efi_tcg2_version {
	u8 major;
	u8 minor;
};

/**
 * struct tdEFI_TCG2_EVENT_HEADER - structure of EFI TCG2 event header
 * @header_size:	size of the event header
 * @header_version:	header version
 * @pcr_index:		index of the PCR that is extended
 * @event_type:		type of the event that is extended
 */
struct efi_tcg2_event_header {
	u32 header_size;
	u16 header_version;
	u32 pcr_index;
	u32 event_type;
} __packed;

/**
 * struct tdEFI_TCG2_EVENT - structure of EFI TCG2 event
 * @size:	total size of the event including the size component, the header
 *		and the event data
 * @header:	event header
 * @event:	event to add
 */
struct efi_tcg2_event {
	u32 size;
	struct efi_tcg2_event_header header;
	u8 event[];
} __packed;

/**
 * struct tdUEFI_IMAGE_LOAD_EVENT - structure of PE/COFF image measurement
 * @image_location_in_memory:	image address
 * @image_length_in_memory:	image size
 * @image_link_time_address:	image link time address
 * @length_of_device_path:	devive path size
 * @device_path:		device path
 */
struct uefi_image_load_event {
	efi_physical_addr_t image_location_in_memory;
	u64 image_length_in_memory;
	u64 image_link_time_address;
	u64 length_of_device_path;
	struct efi_device_path device_path[];
};

/**
 * struct tdEFI_TCG2_BOOT_SERVICE_CAPABILITY - protocol capability information
 * @size:			allocated size of the structure
 * @structure_version:		version of this structure
 * @protocol_version:		version of the EFI TCG2 protocol.
 * @hash_algorithm_bitmap:	supported hash algorithms
 * @supported_event_logs:	bitmap of supported event log formats
 * @tpm_present_flag:		false = TPM not present
 * @max_command_size:		max size (in bytes) of a command
 *				that can be sent to the TPM
 * @max_response_size:		max size (in bytes) of a response that
 *				can be provided by the TPM
 * @manufacturer_id:		4-byte Vendor ID
 * @number_of_pcr_banks:	maximum number of PCR banks
 * @active_pcr_banks:		bitmap of currently active
 *				PCR banks (hashing algorithms).
 */
struct efi_tcg2_boot_service_capability {
	u8 size;
	struct efi_tcg2_version structure_version;
	struct efi_tcg2_version protocol_version;
	efi_tcg_event_algorithm_bitmap hash_algorithm_bitmap;
	efi_tcg_event_log_bitmap supported_event_logs;
	u8 tpm_present_flag;
	u16 max_command_size;
	u16 max_response_size;
	u32 manufacturer_id;
	u32 number_of_pcr_banks;
	efi_tcg_event_algorithm_bitmap active_pcr_banks;
};

/* up to and including the vendor ID (manufacturer_id) field */
#define BOOT_SERVICE_CAPABILITY_MIN \
	offsetof(struct efi_tcg2_boot_service_capability, number_of_pcr_banks)

#define TCG_EFI_SPEC_ID_EVENT_SIGNATURE_03 "Spec ID Event03"
#define TCG_EFI_SPEC_ID_EVENT_SPEC_VERSION_MAJOR_TPM2 2
#define TCG_EFI_SPEC_ID_EVENT_SPEC_VERSION_MINOR_TPM2 0
#define TCG_EFI_SPEC_ID_EVENT_SPEC_VERSION_ERRATA_TPM2 2

/**
 *  struct TCG_EfiSpecIdEventAlgorithmSize - hashing algorithm information
 *
 *  @algorithm_id:	algorithm defined in enum tpm2_algorithms
 *  @digest_size:	size of the algorithm
 */
struct tcg_efi_spec_id_event_algorithm_size {
	u16      algorithm_id;
	u16      digest_size;
} __packed;

/**
 * struct TCG_EfiSpecIDEventStruct - content of the event log header
 *
 * @signature:			signature, set to Spec ID Event03
 * @platform_class:		class defined in TCG ACPI Specification
 *				Client  Common Header.
 * @spec_version_minor:		minor version
 * @spec_version_major:		major version
 * @spec_version_errata:	major version
 * @uintn_size:			size of the efi_uintn_t fields used in various
 *				data structures used in this specification.
 *				0x01 indicates u32  and 0x02  indicates u64
 * @number_of_algorithms:	hashing algorithms used in this event log
 * @digest_sizes:		array of number_of_algorithms pairs
 *				1st member defines the algorithm id
 *				2nd member defines the algorithm size
 * @vendor_info_size:		size in bytes for vendor specific info
 * @vendor_info:		vendor specific info
 */
struct tcg_efi_spec_id_event {
	u8 signature[16];
	u32 platform_class;
	u8 spec_version_minor;
	u8 spec_version_major;
	u8 spec_errata;
	u8 uintn_size;
	u32 number_of_algorithms;
	struct tcg_efi_spec_id_event_algorithm_size digest_sizes[TPM2_NUM_PCR_BANKS];
	u8 vendor_info_size;
	/* U-Boot does not provide any vendor info */
	u8 vendor_info[];
} __packed;

/**
 * struct tdEFI_TCG2_FINAL_EVENTS_TABLE - log entries after Get Event Log
 * @version:		version number for this structure
 * @number_of_events:	number of events recorded after invocation of
 *			GetEventLog()
 * @event:		List of events of type tcg_pcr_event2
 */
struct efi_tcg2_final_events_table {
	u64 version;
	u64 number_of_events;
	struct tcg_pcr_event2 event[];
};

/**
 * struct tdUEFI_VARIABLE_DATA - event log structure of UEFI variable
 * @variable_name:		The vendorGUID parameter in the
 *				GetVariable() API.
 * @unicode_name_length:	The length in CHAR16 of the Unicode name of
 *				the variable.
 * @variable_data_length:	The size of the variable data.
 * @unicode_name:		The CHAR16 unicode name of the variable
 *				without NULL-terminator.
 * @variable_data:		The data parameter of the efi variable
 *				in the GetVariable() API.
 */
struct efi_tcg2_uefi_variable_data {
	efi_guid_t variable_name;
	u64 unicode_name_length;
	u64 variable_data_length;
	u16 unicode_name[1];
	u8 variable_data[1];
};

struct efi_tcg2_protocol {
	efi_status_t (EFIAPI * get_capability)(struct efi_tcg2_protocol *this,
					       struct efi_tcg2_boot_service_capability *capability);
	efi_status_t (EFIAPI * get_eventlog)(struct efi_tcg2_protocol *this,
					     efi_tcg_event_log_format log_format,
					     u64 *event_log_location, u64 *event_log_last_entry,
					     bool *event_log_truncated);
	efi_status_t (EFIAPI * hash_log_extend_event)(struct efi_tcg2_protocol *this,
						      u64 flags,
						      efi_physical_addr_t data_to_hash,
						      u64 data_to_hash_len,
						      struct efi_tcg2_event *efi_tcg_event);
	efi_status_t (EFIAPI * submit_command)(struct efi_tcg2_protocol *this,
					       u32 input_parameter_block_size,
					       u8 *input_parameter_block,
					       u32 output_parameter_block_size,
					       u8 *output_parameter_block);
	efi_status_t (EFIAPI * get_active_pcr_banks)(struct efi_tcg2_protocol *this,
						     u32 *active_pcr_banks);
	efi_status_t (EFIAPI * set_active_pcr_banks)(struct efi_tcg2_protocol *this,
						     u32 active_pcr_banks);
	efi_status_t (EFIAPI * get_result_of_set_active_pcr_banks)(struct efi_tcg2_protocol *this,
								   u32 *operation_present,
								   u32 *response);
};
#endif
