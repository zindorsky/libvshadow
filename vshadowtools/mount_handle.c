/*
 * Mount handle
 *
 * Copyright (C) 2011-2016, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <memory.h>
#include <types.h>

#include "mount_handle.h"
#include "vshadowtools_libbfio.h"
#include "vshadowtools_libcerror.h"
#include "vshadowtools_libcstring.h"
#include "vshadowtools_libcsystem.h"
#include "vshadowtools_libvshadow.h"

#if !defined( LIBVSHADOW_HAVE_BFIO )
extern \
int libvshadow_volume_open_file_io_handle(
     libvshadow_volume_t *volume,
     libbfio_handle_t *file_io_handle,
     int access_flags,
     libvshadow_error_t **error );

extern \
int libvshadow_check_volume_signature_file_io_handle(
     libbfio_handle_t *file_io_handle,
     libcerror_error_t **error );
#endif

/* Creates a mount handle
 * Make sure the value mount_handle is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int mount_handle_initialize(
     mount_handle_t **mount_handle,
     libcerror_error_t **error )
{
	static char *function = "mount_handle_initialize";

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	if( *mount_handle != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid mount handle value already set.",
		 function );

		return( -1 );
	}
	*mount_handle = memory_allocate_structure(
	                 mount_handle_t );

	if( *mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create mount handle.",
		 function );

		memory_free(
		 *mount_handle );

		*mount_handle = NULL;

		return( -1 );
	}
	if( memory_set(
	     *mount_handle,
	     0,
	     sizeof( mount_handle_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear mount handle.",
		 function );

		goto on_error;
	}
	if( libbfio_file_range_initialize(
	     &( ( *mount_handle )->input_file_io_handle ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to initialize input file IO handle.",
		 function );

		goto on_error;
	}
	if( libvshadow_volume_initialize(
	     &( ( *mount_handle )->input_volume ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to initialize input volume.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *mount_handle != NULL )
	{
		if( ( *mount_handle )->input_file_io_handle != NULL )
		{
			libbfio_handle_free(
			 &( ( *mount_handle )->input_file_io_handle ),
			 NULL );
		}
		memory_free(
		 *mount_handle );

		*mount_handle = NULL;
	}
	return( -1 );
}

/* Frees a mount handle
 * Returns 1 if successful or -1 on error
 */
int mount_handle_free(
     mount_handle_t **mount_handle,
     libcerror_error_t **error )
{
	static char *function = "mount_handle_free";
	int result            = 1;

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	if( *mount_handle != NULL )
	{
		if( ( *mount_handle )->input_stores != NULL )
		{
			if( mount_handle_close_input(
			     *mount_handle,
			     error ) != 0 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_IO,
				 LIBCERROR_IO_ERROR_CLOSE_FAILED,
				 "%s: unable to close mount handle.",
				 function );

				result = -1;
			}
		}
		if( libvshadow_volume_free(
		     &( ( *mount_handle )->input_volume ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free input volume.",
			 function );

			result = -1;
		}
		if( libbfio_handle_free(
		     &( ( *mount_handle )->input_file_io_handle ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free input file IO handle.",
			 function );

			result = -1;
		}
		memory_free(
		 *mount_handle );

		*mount_handle = NULL;
	}
	return( result );
}

/* Signals the mount handle to abort
 * Returns 1 if successful or -1 on error
 */
int mount_handle_signal_abort(
     mount_handle_t *mount_handle,
     libcerror_error_t **error )
{
	static char *function = "mount_handle_signal_abort";

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	if( mount_handle->input_volume != NULL )
	{
		if( libvshadow_volume_signal_abort(
		     mount_handle->input_volume,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to signal input volume to abort.",
			 function );

			return( -1 );
		}
	}
	return( 1 );
}

/* Sets the volume offset
 * Returns 1 if successful or -1 on error
 */
int mount_handle_set_volume_offset(
     mount_handle_t *mount_handle,
     const libcstring_system_character_t *string,
     libcerror_error_t **error )
{
	static char *function = "mount_handle_set_volume_offset";
	size_t string_length  = 0;
	uint64_t value_64bit  = 0;

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	string_length = libcstring_system_string_length(
	                 string );

	if( libcsystem_string_decimal_copy_to_64_bit(
	     string,
	     string_length + 1,
	     &value_64bit,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_COPY_FAILED,
		 "%s: unable to copy string to 64-bit decimal.",
		 function );

		return( -1 );
	}
	mount_handle->volume_offset = (off64_t) value_64bit;

	return( 1 );
}

/* Opens the mount handle
 * Returns 1 if successful, 0 if no VSS enabled volume was found or -1 on error
 */
int mount_handle_open_input(
     mount_handle_t *mount_handle,
     const libcstring_system_character_t *filename,
     libcerror_error_t **error )
{
	static char *function  = "mount_handle_open";
	size_t filename_length = 0;
	int result             = 0;

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	filename_length = libcstring_system_string_length(
	                   filename );

#if defined( LIBCSTRING_HAVE_WIDE_SYSTEM_CHARACTER )
	if( libbfio_file_range_set_name_wide(
	     mount_handle->input_file_io_handle,
	     filename,
	     filename_length,
	     error ) != 1 )
#else
	if( libbfio_file_range_set_name(
	     mount_handle->input_file_io_handle,
	     filename,
	     filename_length,
	     error ) != 1 )
#endif
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to set file name.",
		 function );

		return( -1 );
	}
	if( libbfio_file_range_set(
	     mount_handle->input_file_io_handle,
	     mount_handle->volume_offset,
	     0,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_OPEN_FAILED,
		 "%s: unable to set volume offset.",
		 function );

		return( -1 );
	}
	result = libvshadow_check_volume_signature_file_io_handle(
	          mount_handle->input_file_io_handle,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if volume has a VSS signature.",
		 function );

		return( -1 );
	}
	else if( result != 0 )
	{
		if( libvshadow_volume_open_file_io_handle(
		     mount_handle->input_volume,
		     mount_handle->input_file_io_handle,
		     LIBVSHADOW_OPEN_READ,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_OPEN_FAILED,
			 "%s: unable to open input volume.",
			 function );

			return( -1 );
		}
		if( libvshadow_volume_get_number_of_stores(
		     mount_handle->input_volume,
		     &( mount_handle->number_of_input_stores ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of stores.",
			 function );

			return( -1 );
		}
		mount_handle->input_stores = (libvshadow_store_t **) memory_allocate(
		                                                      sizeof( libvshadow_store_t * ) * mount_handle->number_of_input_stores );

		if( mount_handle->input_stores == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
			 "%s: unable to create input stores.",
			 function );

			goto on_error;
		}
		if( memory_set(
		     mount_handle->input_stores,
		     0,
		     sizeof( libvshadow_store_t * ) * mount_handle->number_of_input_stores ) == NULL )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_MEMORY,
			 LIBCERROR_MEMORY_ERROR_SET_FAILED,
			 "%s: unable to clear input stores.",
			 function );

			goto on_error;
		}
	}
	return( result );

on_error:
	if( mount_handle->input_stores != NULL )
	{
		memory_free(
		 mount_handle->input_stores );
	}
	return( -1 );
}

/* Closes the mount handle
 * Returns the 0 if succesful or -1 on error
 */
int mount_handle_close_input(
     mount_handle_t *mount_handle,
     libcerror_error_t **error )
{
	static char *function = "mount_handle_close_input";
	int result            = 0;
	int store_index       = 0;

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	if( mount_handle->input_stores != NULL )
	{
		for( store_index = 0;
		     store_index < mount_handle->number_of_input_stores;
		     store_index++ )
		{
			if( mount_handle->input_stores[ store_index ] != NULL )
			{
				if( libvshadow_store_free(
			             &( mount_handle->input_stores[ store_index ] ),
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
					 "%s: unable to free input store: %d.",
					 function,
					 store_index );

					result = -1;
				}
			}
		}
		memory_free(
		 mount_handle->input_stores );
	}
	if( libvshadow_volume_close(
	     mount_handle->input_volume,
	     error ) != 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_CLOSE_FAILED,
		 "%s: unable to close input volume.",
		 function );

		result = -1;
	}
	return( result );
}

/* Read a buffer from the specified input store
 [*] Returns the number of bytes read if successful or -1 on error
 */
ssize_t mount_handle_read_buffer(
         mount_handle_t *mount_handle,
         int store_index,
         uint8_t *buffer,
         size_t size,
         libcerror_error_t **error )
{
	static char *function = "mount_handle_read_buffer";
	ssize_t read_count    = 0;

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	if( ( store_index < 0 )
	 || ( store_index >= mount_handle->number_of_input_stores ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: unsupported to store index value out of bounds.",
		 function );

		return( -1 );
	}
	if( mount_handle->input_stores[ store_index ] == NULL )
	{
		if( libvshadow_volume_get_store(
		     mount_handle->input_volume,
		     store_index,
		     &( mount_handle->input_stores[ store_index ] ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: unable to retrieve input store: %d from input volume.",
			 function,
			 store_index );

			return( -1 );
		}
	}
	read_count = libvshadow_store_read_buffer(
	              mount_handle->input_stores[ store_index ],
	              buffer,
	              size,
	              error );

	if( read_count == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read buffer from input store: %d.",
		 function,
		 store_index );

		return( -1 );
	}
	return( read_count );
}

/* Seeks a specific offset from the specified input store
 [*] Returns the offset if successful or -1 on error
 */
off64_t mount_handle_seek_offset(
         mount_handle_t *mount_handle,
         int store_index,
         off64_t offset,
         int whence,
         libcerror_error_t **error )
{
	static char *function = "mount_handle_seek_offset";

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	if( ( store_index < 0 )
	 || ( store_index >= mount_handle->number_of_input_stores ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: unsupported to store index value out of bounds: %d.",
		 function,
		 store_index );

		return( -1 );
	}
	if( mount_handle->input_stores[ store_index ] == NULL )
	{
		if( libvshadow_volume_get_store(
		     mount_handle->input_volume,
		     store_index,
		     &( mount_handle->input_stores[ store_index ] ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
			 "%s: unable to retrieve input store: %d from input volume.",
			 function,
			 store_index );

			return( -1 );
		}
	}
	offset = libvshadow_store_seek_offset(
	          mount_handle->input_stores[ store_index ],
	          offset,
	          whence,
	          error );

	if( offset == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_SEEK_FAILED,
		 "%s: unable to seek offset in input store: %d.",
		 function,
		 store_index );

		return( -1 );
	}
	return( offset );
}

/* Retrieves the size of the input volume
 * Returns 1 if successful or -1 on error
 */
int mount_handle_get_size(
     mount_handle_t *mount_handle,
     size64_t *size,
     libcerror_error_t **error )
{
	static char *function = "mount_handle_get_size";

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	if( libvshadow_volume_get_size(
	     mount_handle->input_volume,
	     size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve size from input volume.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the number of stores of the input volume
 * Returns 1 if successful or -1 on error
 */
int mount_handle_get_number_of_stores(
     mount_handle_t *mount_handle,
     int *number_of_stores,
     libcerror_error_t **error )
{
	static char *function = "mount_handle_get_number_of_stores";

	if( mount_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid mount handle.",
		 function );

		return( -1 );
	}
	if( number_of_stores == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid number of stores.",
		 function );

		return( -1 );
	}
	*number_of_stores = mount_handle->number_of_input_stores;

	return( 1 );
}

