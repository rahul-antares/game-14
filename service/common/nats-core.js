const { EventEmitter } = require('events');
const  nats            = require('nats');

const ErrorCode   = nats.ErrorCode;
const stringCodec = nats.StringCodec();
const jsonCodec   = nats.JSONCodec();

class NatsCore {
	static nc = null;
	static emitter = new EventEmitter ();

	static async init (opts = {}) {
		if (this.nc)
			return this.emitter.emit ('nats.already-initalized');

		this.nc = await nats.connect ({
			maxReconnectAttempts: -1,
			reconnectTimeWait   : 250,
			...opts,
		});

		this.emitter.emit ('nats.connected', this.nc.getServer ());

		(async () => {
			for await (const s of this.nc.status()) {
				/* Life cycle details => https://github.com/nats-io/nats.js#lifecycleinformational-events */
				switch (s.type) {
					case 'error':
						this.emitter.emit ('nats.error', s.data);
						break;
					case 'disconnect':
						this.emitter.emit ('nats.disconnected', s.data);
						break;
					case 'reconnecting':
						this.emitter.emit ('nats.reconnecting', s.data);
						break;
					case 'reconnect':
						this.emitter.emit ('nats.reconnected', s.data);
						break;
					case 'update':
						this.emitter.emit ('nats.updated', s.data);
						break;
				}
			}
		})();

		this.nc.closed ()
			.then ((err) => {
				if (err)
					this.emitter.emit ('nats.error', err);
			})
		;

		return this.nc;
	}

	static get nc () { this.nc; }
	static flush () { return this.nc.flush (); }
	static close () { return this.nc.close (); this.nc = null; }
	static drain () { return this.nc.drain (); this.nc = null; }

	static openChannel (channel, handleMsgCallback, opt = {}) {
		if (!this.nc)
			throw new NatsError ('nats not connected');

		const sub = this.nc.subscribe (channel, opt.subscribe || {});

		(async () => {
			for await (const msg of sub) {
				try {
					const data    = jsonCodec.decode (msg.data);
					const subject = msg.subject;
					handleMsgCallback (null, subject, data, msg.reply, msg);
				}
				catch (err) {
					const subject = msg && msg.subject || channel
					this.emitter.emit ('nats.subscription-error', err, { channel:subject });
					handleMsgCallback (err, subject, null, null, null);
				}
			} 
		})();
	}

	static openChannelBinary (channel, handleMsgCallback, opt = {}) {
		if (!this.nc)
			throw new NatsError ('nats not connected');

		const sub = this.nc.subscribe (channel);

		(async () => {
			for await (const msg of sub) {
				try {
					const data    = msg.data;
					const subject = msg.subject;
					handleMsgCallback (null, subject, data, msg.reply, msg);
				}
				catch (err) {
					const subject = msg && msg.subject || channel
					this.emitter.emit ('nats.subscription-error', err, { channel:subject });
					handleMsgCallback (err, subject, null, null, null);
				}
			} 
		})();
	}

	static on (ev, handler) { this.emitter.on (ev, handler); }

	static send (channel, json, opt = {}) {
		if (!this.nc)
			throw new NatsError ('nats not connected');

		return this.nc.publish (channel, jsonCodec.encode (json), opt);
	}

	static sendRaw (channel, buffer, opt = {}) {
		if (!this.nc)
			throw new NatsError ('nats not connected');

		return this.nc.publish (channel, buffer, opt);
	}

	static reply   = this.send;
	static publish = this.send;

	static async request (channel, msg, options) {
		if (!this.nc)
			throw new NatsError ('nats not connected');

		const inbox = nats.createInbox();
		const sub   = this.nc.subscribe(inbox, { max: 1 });
		let response;

		this.send (channel, msg, { reply: inbox });

		await (async () => {
			for await (const msg of sub) {

				if (!msg.data.length && msg.headers.status === ErrorCode.NoResponders)
					throw new NatsError ('no responders');

				response = jsonCodec.decode (msg.data);
			}
		})();

		return response;
	}

	static requestWithTimeout (subject, msg, timeout) {
		return this.request (subject, msg, { timeout });
	}
}


class NatsError extends Error {
	constructor(...args) {
		super(...args);
		this.name = 'NATSERROR';
	}
}

module.exports = NatsCore;
