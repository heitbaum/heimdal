/*
 * Copyright (c) 2004, PADL Software Pty Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of PADL Software nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY PADL SOFTWARE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL PADL SOFTWARE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "spnego_locl.h"

OM_uint32 GSSAPI_CALLCONV _gss_spnego_process_context_token
           (OM_uint32 *minor_status,
            gss_const_ctx_id_t context_handle,
            const gss_buffer_t token_buffer
           )
{
    gss_ctx_id_t context;
    gssspnego_ctx ctx;
    OM_uint32 ret;

    if (context_handle == GSS_C_NO_CONTEXT)
	return GSS_S_NO_CONTEXT;

    context = (gss_ctx_id_t)context_handle;
    ctx = (gssspnego_ctx)context_handle;

    HEIMDAL_MUTEX_lock(&ctx->ctx_id_mutex);

    ret = gss_process_context_token(minor_status,
				    ctx->negotiated_ctx_id,
				    token_buffer);
    if (ret != GSS_S_COMPLETE) {
	HEIMDAL_MUTEX_unlock(&ctx->ctx_id_mutex);
	return ret;
    }

    ctx->negotiated_ctx_id = GSS_C_NO_CONTEXT;

    return _gss_spnego_internal_delete_sec_context(minor_status,
					   &context,
					   GSS_C_NO_BUFFER);
}

OM_uint32 GSSAPI_CALLCONV _gss_spnego_delete_sec_context
           (OM_uint32 *minor_status,
            gss_ctx_id_t *context_handle,
            gss_buffer_t output_token
           )
{
    gssspnego_ctx ctx;

    if (context_handle == NULL || *context_handle == GSS_C_NO_CONTEXT)
	return GSS_S_NO_CONTEXT;

    ctx = (gssspnego_ctx)*context_handle;

    HEIMDAL_MUTEX_lock(&ctx->ctx_id_mutex);

    return _gss_spnego_internal_delete_sec_context(minor_status,
						   context_handle,
						   output_token);
}

OM_uint32 GSSAPI_CALLCONV _gss_spnego_context_time
           (OM_uint32 *minor_status,
            gss_const_ctx_id_t context_handle,
            OM_uint32 *time_rec
           )
{
    gssspnego_ctx ctx;
    *minor_status = 0;

    if (context_handle == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    ctx = (gssspnego_ctx)context_handle;

    if (ctx->negotiated_ctx_id == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    return gss_context_time(minor_status,
			    ctx->negotiated_ctx_id,
			    time_rec);
}

OM_uint32 GSSAPI_CALLCONV _gss_spnego_get_mic
           (OM_uint32 *minor_status,
            gss_const_ctx_id_t context_handle,
            gss_qop_t qop_req,
            const gss_buffer_t message_buffer,
            gss_buffer_t message_token
           )
{
    gssspnego_ctx ctx;

    *minor_status = 0;

    if (context_handle == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    ctx = (gssspnego_ctx)context_handle;

    if (ctx->negotiated_ctx_id == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    return gss_get_mic(minor_status, ctx->negotiated_ctx_id,
		       qop_req, message_buffer, message_token);
}

OM_uint32 GSSAPI_CALLCONV _gss_spnego_verify_mic
           (OM_uint32 * minor_status,
            gss_const_ctx_id_t context_handle,
            const gss_buffer_t message_buffer,
            const gss_buffer_t token_buffer,
            gss_qop_t * qop_state
           )
{
    gssspnego_ctx ctx;

    *minor_status = 0;

    if (context_handle == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    ctx = (gssspnego_ctx)context_handle;

    if (ctx->negotiated_ctx_id == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    return gss_verify_mic(minor_status,
			  ctx->negotiated_ctx_id,
			  message_buffer,
			  token_buffer,
			  qop_state);
}

OM_uint32 GSSAPI_CALLCONV _gss_spnego_wrap
           (OM_uint32 * minor_status,
            gss_const_ctx_id_t context_handle,
            int conf_req_flag,
            gss_qop_t qop_req,
            const gss_buffer_t input_message_buffer,
            int * conf_state,
            gss_buffer_t output_message_buffer
           )
{
    gssspnego_ctx ctx;

    *minor_status = 0;

    if (context_handle == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    ctx = (gssspnego_ctx)context_handle;

    if (ctx->negotiated_ctx_id == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    return gss_wrap(minor_status,
		    ctx->negotiated_ctx_id,
		    conf_req_flag,
		    qop_req,
		    input_message_buffer,
		    conf_state,
		    output_message_buffer);
}

OM_uint32 GSSAPI_CALLCONV _gss_spnego_unwrap
           (OM_uint32 * minor_status,
            gss_const_ctx_id_t context_handle,
            const gss_buffer_t input_message_buffer,
            gss_buffer_t output_message_buffer,
            int * conf_state,
            gss_qop_t * qop_state
           )
{
    gssspnego_ctx ctx;

    *minor_status = 0;

    if (context_handle == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    ctx = (gssspnego_ctx)context_handle;

    if (ctx->negotiated_ctx_id == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    return gss_unwrap(minor_status,
		      ctx->negotiated_ctx_id,
		      input_message_buffer,
		      output_message_buffer,
		      conf_state,
		      qop_state);
}

OM_uint32 GSSAPI_CALLCONV _gss_spnego_inquire_context (
            OM_uint32 * minor_status,
            gss_const_ctx_id_t context_handle,
            gss_name_t * src_name,
            gss_name_t * targ_name,
            OM_uint32 * lifetime_rec,
            gss_OID * mech_type,
            OM_uint32 * ctx_flags,
            int * locally_initiated,
            int * open_context
           )
{
    gssspnego_ctx ctx;
    OM_uint32 maj_stat;

    *minor_status = 0;

    if (context_handle == GSS_C_NO_CONTEXT)
	return GSS_S_NO_CONTEXT;

    ctx = (gssspnego_ctx)context_handle;

    if (ctx->negotiated_ctx_id == GSS_C_NO_CONTEXT)
	return GSS_S_NO_CONTEXT;

    maj_stat = gss_inquire_context(minor_status,
				   ctx->negotiated_ctx_id,
				   src_name,
				   targ_name,
				   lifetime_rec,
				   mech_type,
				   ctx_flags,
				   locally_initiated,
				   open_context);

    if (open_context)
	*open_context = gssspnego_ctx_complete_p(ctx);

    return maj_stat;
}

OM_uint32 GSSAPI_CALLCONV _gss_spnego_wrap_size_limit (
            OM_uint32 * minor_status,
            gss_const_ctx_id_t context_handle,
            int conf_req_flag,
            gss_qop_t qop_req,
            OM_uint32 req_output_size,
            OM_uint32 * max_input_size
           )
{
    gssspnego_ctx ctx;

    *minor_status = 0;

    if (context_handle == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    ctx = (gssspnego_ctx)context_handle;

    if (ctx->negotiated_ctx_id == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    return gss_wrap_size_limit(minor_status,
			       ctx->negotiated_ctx_id,
			       conf_req_flag,
			       qop_req,
			       req_output_size,
			       max_input_size);
}

OM_uint32 GSSAPI_CALLCONV _gss_spnego_export_sec_context (
            OM_uint32 * minor_status,
            gss_ctx_id_t * context_handle,
            gss_buffer_t interprocess_token
           )
{
    gssspnego_ctx ctx;
    OM_uint32 major_status;

    *minor_status = 0;

    if (context_handle == NULL)
	return GSS_S_NO_CONTEXT;

    ctx = (gssspnego_ctx)*context_handle;

    if (ctx == NULL)
	return GSS_S_NO_CONTEXT;

    HEIMDAL_MUTEX_lock(&ctx->ctx_id_mutex);

    /*
     * Partial context export is only supported on the acceptor side, as we
     * cannot represent the initiator function pointer state in an exported
     * token, and also because it is mostly useful for acceptors which need
     * to manage multiple initiator states.
     */
    if (ctx->flags.local && !gssspnego_ctx_complete_p(ctx)) {
	major_status = GSS_S_NO_CONTEXT;
	goto out;
    }

    major_status = _gss_spnego_export_sec_context_internal(minor_status,
							   ctx,
							   interprocess_token);

out:
    if (major_status == GSS_S_COMPLETE)
	major_status = _gss_spnego_internal_delete_sec_context(minor_status,
							       context_handle,
							       GSS_C_NO_BUFFER);
    else
	HEIMDAL_MUTEX_unlock(&ctx->ctx_id_mutex);

    return major_status;
}

OM_uint32 GSSAPI_CALLCONV _gss_spnego_import_sec_context (
            OM_uint32 * minor_status,
            const gss_buffer_t interprocess_token,
            gss_ctx_id_t *context_handle
           )
{
    return _gss_spnego_import_sec_context_internal(minor_status,
						   interprocess_token,
						   (gssspnego_ctx *)context_handle);
}

OM_uint32 GSSAPI_CALLCONV _gss_spnego_inquire_names_for_mech (
            OM_uint32 * minor_status,
            const gss_OID mechanism,
            gss_OID_set * name_types
           )
{
    gss_OID_set mechs, names, n;
    OM_uint32 ret, junk;
    size_t i, j;

    *name_types = NULL;

    ret = _gss_spnego_indicate_mechs(minor_status, &mechs);
    if (ret != GSS_S_COMPLETE)
	return ret;

    ret = gss_create_empty_oid_set(minor_status, &names);
    if (ret != GSS_S_COMPLETE)
	goto out;

    for (i = 0; i < mechs->count; i++) {
	ret = gss_inquire_names_for_mech(minor_status,
					 &mechs->elements[i],
					 &n);
	if (ret)
	    continue;

	for (j = 0; j < n->count; j++)
	    gss_add_oid_set_member(minor_status,
				   &n->elements[j],
				   &names);
	gss_release_oid_set(&junk, &n);
    }

    ret = GSS_S_COMPLETE;
    *name_types = names;
out:

    gss_release_oid_set(&junk, &mechs);

    return ret;
}

OM_uint32 GSSAPI_CALLCONV
_gss_spnego_wrap_iov(OM_uint32 * minor_status,
		     gss_ctx_id_t  context_handle,
		     int conf_req_flag,
		     gss_qop_t qop_req,
		     int * conf_state,
		     gss_iov_buffer_desc *iov,
		     int iov_count)
{
    gssspnego_ctx ctx = (gssspnego_ctx)context_handle;

    *minor_status = 0;

    if (ctx == NULL || ctx->negotiated_ctx_id == GSS_C_NO_CONTEXT)
	return GSS_S_NO_CONTEXT;

    return gss_wrap_iov(minor_status, ctx->negotiated_ctx_id,
			conf_req_flag, qop_req, conf_state,
			iov, iov_count);
}

OM_uint32 GSSAPI_CALLCONV
_gss_spnego_unwrap_iov(OM_uint32 *minor_status,
		       gss_ctx_id_t context_handle,
		       int *conf_state,
		       gss_qop_t *qop_state,
		       gss_iov_buffer_desc *iov,
		       int iov_count)
{
    gssspnego_ctx ctx = (gssspnego_ctx)context_handle;

    *minor_status = 0;

    if (ctx == NULL || ctx->negotiated_ctx_id == GSS_C_NO_CONTEXT)
	return GSS_S_NO_CONTEXT;

    return gss_unwrap_iov(minor_status,
			  ctx->negotiated_ctx_id,
			  conf_state, qop_state,
			  iov, iov_count);
}

OM_uint32 GSSAPI_CALLCONV
_gss_spnego_wrap_iov_length(OM_uint32 * minor_status,
			    gss_ctx_id_t context_handle,
			    int conf_req_flag,
			    gss_qop_t qop_req,
			    int *conf_state,
			    gss_iov_buffer_desc *iov,
			    int iov_count)
{
    gssspnego_ctx ctx = (gssspnego_ctx)context_handle;

    *minor_status = 0;

    if (ctx == NULL || ctx->negotiated_ctx_id == GSS_C_NO_CONTEXT)
	return GSS_S_NO_CONTEXT;

    return gss_wrap_iov_length(minor_status, ctx->negotiated_ctx_id,
			       conf_req_flag, qop_req, conf_state,
			       iov, iov_count);
}

#if 0
OM_uint32 GSSAPI_CALLCONV _gss_spnego_complete_auth_token
           (OM_uint32 * minor_status,
            gss_const_ctx_id_t context_handle,
	    gss_buffer_t input_message_buffer)
{
    gssspnego_ctx ctx;

    *minor_status = 0;

    if (context_handle == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    ctx = (gssspnego_ctx)context_handle;

    if (ctx->negotiated_ctx_id == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    return gss_complete_auth_token(minor_status,
				   ctx->negotiated_ctx_id,
				   input_message_buffer);
}
#endif

OM_uint32 GSSAPI_CALLCONV _gss_spnego_inquire_sec_context_by_oid
           (OM_uint32 * minor_status,
            gss_const_ctx_id_t context_handle,
            const gss_OID desired_object,
            gss_buffer_set_t *data_set)
{
    gssspnego_ctx ctx;

    *minor_status = 0;

    if (context_handle == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    ctx = (gssspnego_ctx)context_handle;

    if (ctx->negotiated_ctx_id == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    return gss_inquire_sec_context_by_oid(minor_status,
					  ctx->negotiated_ctx_id,
					  desired_object,
					  data_set);
}

OM_uint32 GSSAPI_CALLCONV _gss_spnego_set_sec_context_option
           (OM_uint32 * minor_status,
            gss_ctx_id_t * context_handle,
            const gss_OID desired_object,
            const gss_buffer_t value)
{
    gssspnego_ctx ctx;

    *minor_status = 0;

    if (context_handle == NULL || *context_handle == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    ctx = (gssspnego_ctx)*context_handle;

    if (ctx->negotiated_ctx_id == GSS_C_NO_CONTEXT) {
	return GSS_S_NO_CONTEXT;
    }

    return gss_set_sec_context_option(minor_status,
				      &ctx->negotiated_ctx_id,
				      desired_object,
				      value);
}


OM_uint32 GSSAPI_CALLCONV
_gss_spnego_pseudo_random(OM_uint32 *minor_status,
			  gss_ctx_id_t context_handle,
			  int prf_key,
			  const gss_buffer_t prf_in,
			  ssize_t desired_output_len,
			  gss_buffer_t prf_out)
{
    gssspnego_ctx ctx;

    *minor_status = 0;

    if (context_handle == GSS_C_NO_CONTEXT)
	return GSS_S_NO_CONTEXT;

    ctx = (gssspnego_ctx)context_handle;

    if (ctx->negotiated_ctx_id == GSS_C_NO_CONTEXT)
	return GSS_S_NO_CONTEXT;

    return gss_pseudo_random(minor_status,
			     ctx->negotiated_ctx_id,
			     prf_key,
			     prf_in,
			     desired_output_len,
			     prf_out);
}
