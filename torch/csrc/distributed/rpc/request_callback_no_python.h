#pragma once

#include <torch/csrc/distributed/rpc/message.h>
#include <torch/csrc/distributed/rpc/request_callback.h>
#include <torch/csrc/distributed/rpc/rpc_command_base.h>
#include <torch/csrc/distributed/rpc/rref_impl.h>
#include <torch/csrc/distributed/rpc/script_call.h>
#include <torch/csrc/distributed/rpc/script_remote_call.h>

namespace torch {
namespace distributed {
namespace rpc {

// RequestCallback implementation with no Python dependencies.
class TORCH_API RequestCallbackNoPython : public RequestCallback {
 public:
  c10::intrusive_ptr<JitFuture> processMessage(
      Message& request,
      std::shared_ptr<LazyStreamContext> ctx) const override;

 protected:
  virtual std::unique_ptr<RpcCommandBase> deserializePythonRpcCommand(
      std::unique_ptr<RpcCommandBase> rpc,
      const MessageType& messageType) const;

  virtual void processScriptCall(
      RpcCommandBase& rpc,
      const std::function<void(Message)>& markComplete,
      const c10::intrusive_ptr<JitFuture>& responseFuture) const;

  void processScriptCallOp(
      ScriptCall& scriptCall,
      const std::function<void(Message)>& markComplete,
      std::vector<at::IValue>& stack) const;

  virtual void processPythonCall(
      RpcCommandBase& rpc,
      const std::function<void(Message)>& markComplete,
      const c10::intrusive_ptr<JitFuture>& responseFuture) const;

  virtual TypePtr getScriptRemoteCallType(
      ScriptRemoteCall& scriptRemoteCall) const;

  virtual void processScriptRemoteCall(
      ScriptRemoteCall& scriptRemoteCall,
      const std::function<void(void)>& postProcessing,
      std::vector<at::IValue>& stack,
      const c10::intrusive_ptr<OwnerRRef>& ownerRRef) const;

  void processBaseScriptRemoteCall(
      RpcCommandBase& rpc,
      const std::function<void(Message)>& markComplete,
      const c10::intrusive_ptr<JitFuture>& responseFuture) const;

  void processScriptRemoteCallOp(
      ScriptRemoteCall& scriptRemoteCall,
      const std::function<void(void)>& postProcessing,
      std::vector<at::IValue>& stack,
      const c10::intrusive_ptr<OwnerRRef>& ownerRRef) const;

  virtual void processPythonRemoteCall(
      RpcCommandBase& rpc,
      const std::function<void(Message)>& markComplete,
      const c10::intrusive_ptr<JitFuture>& responseFuture,
      std::shared_ptr<LazyStreamContext> ctx) const;

  void processScriptRRefFetchCall(
      RpcCommandBase& rpc,
      const std::function<void(Message)>& markComplete,
      const c10::intrusive_ptr<JitFuture>& responseFuture) const;

  virtual void processPythonRRefFetchCall(
      RpcCommandBase& rpc,
      const c10::intrusive_ptr<JitFuture>& responseFuture,
      std::shared_ptr<LazyStreamContext> ctx) const;

  void processRRefUserDelete(
      RpcCommandBase& rpc,
      const std::function<void(Message)>& markComplete) const;

  void processRRefChildAccept(
      RpcCommandBase& rpc,
      const std::function<void(Message)>& markComplete) const;

  void processRRefForkRequest(
      RpcCommandBase& rpc,
      const std::function<void(Message)>& markComplete) const;

  c10::intrusive_ptr<JitFuture> processForwardAutogradReq(
      RpcCommandBase& rpc,
      std::shared_ptr<LazyStreamContext> ctx) const;

  void processBackwardAutogradReq(
      RpcCommandBase& rpc,
      const c10::intrusive_ptr<JitFuture>& responseFuture) const;

  void processCleanupAutogradContextReq(
      RpcCommandBase& rpc,
      const std::function<void(Message)>& markComplete) const;

  c10::intrusive_ptr<JitFuture> processRunWithProfilingReq(
      RpcCommandBase& rpc) const;

  virtual void handleRRefDelete(c10::intrusive_ptr<RRef>& rref) const;

  c10::intrusive_ptr<JitFuture> processRpc(
      RpcCommandBase& rpc,
      const MessageType& messageType,
      std::shared_ptr<LazyStreamContext> ctx) const;

  virtual c10::intrusive_ptr<JitFuture> processRpcWithErrors(
      RpcCommandBase& rpc,
      const MessageType& messageType,
      std::shared_ptr<LazyStreamContext> ctx) const;

  c10::intrusive_ptr<Message> handleError(
      const std::exception& e,
      const MessageType messageType,
      int64_t messageId) const;

  virtual bool cudaAvailable() const;

  virtual void processRRefBackward(
      RpcCommandBase& rpc,
      const c10::intrusive_ptr<JitFuture>& responseFuture) const;

  // Helpers to convert various kinds of objects into already-completed futures.

  c10::intrusive_ptr<JitFuture> asFuture(IValue value, TypePtr type) const;

  c10::intrusive_ptr<JitFuture> asFuture(
      c10::intrusive_ptr<Message> message) const;

  c10::intrusive_ptr<JitFuture> asFuture(Message message) const;

  c10::intrusive_ptr<JitFuture> asFuture(std::exception_ptr err) const;
};

} // namespace rpc
} // namespace distributed
} // namespace torch
