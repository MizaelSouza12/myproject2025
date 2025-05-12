/**
 * RemoteAccessLinkGenerator Class
 * 
 * Gera e valida links para acesso remoto ao sistema.
 * Resolve problemas com links expirados prematuramente e validação inconsistente.
 */
const crypto = require('crypto');
const jwt = require('jsonwebtoken');

class RemoteAccessLinkGenerator {
  constructor(options = {}) {
    this.baseUrl = options.baseUrl || 'https://remote.wydbr.com/access';
    this.tokenSecret = process.env.LINK_TOKEN_SECRET || 'your-token-secret-should-be-set-in-env';
    this.defaultExpiry = options.defaultExpiry || 24; // horas
    
    if (process.env.LINK_TOKEN_SECRET) {
      console.log('RemoteAccessLinkGenerator inicializado com chave secreta do ambiente');
    } else {
      console.warn('AVISO: LINK_TOKEN_SECRET não encontrado no ambiente, usando valor padrão inseguro');
    }
  }
  
  // Gera um novo link de acesso
  generateLink(userId, permissions, expiryHours = this.defaultExpiry) {
    if (!userId) {
      throw new Error('UserId é obrigatório para gerar um link');
    }
    
    const expiryTime = Date.now() + (expiryHours * 60 * 60 * 1000);
    
    // Gerar token JWT seguro
    const token = this.generateSecureToken(userId, permissions, expiryTime);
    
    // Construir URL completa com todos parâmetros necessários
    const url = `${this.baseUrl}/${token}?uid=${userId}&exp=${expiryTime}`;
    
    console.log(`Link de acesso remoto gerado para usuário ${userId}, expira em ${expiryHours} horas`);
    
    return {
      url,
      expiresAt: new Date(expiryTime),
      permissions,
      token
    };
  }
  
  // Valida um link de acesso
  validateLink(token, userId = null) {
    try {
      if (!token) {
        return { valid: false, reason: 'no_token' };
      }
      
      // Verificar assinatura do token
      const decoded = this.verifyToken(token);
      
      // Verificar se não expirou
      if (decoded.exp < Date.now()) {
        return { valid: false, reason: 'expired', expiryTime: new Date(decoded.exp) };
      }
      
      // Verificar userId se fornecido
      if (userId && decoded.userId !== userId) {
        return { valid: false, reason: 'user_mismatch' };
      }
      
      return { 
        valid: true,
        userId: decoded.userId,
        permissions: decoded.permissions,
        expiresAt: new Date(decoded.exp)
      };
    } catch (error) {
      return { 
        valid: false, 
        reason: 'invalid_token', 
        error: error.message 
      };
    }
  }
  
  // Renova um token existente
  renewToken(existingToken, additionalHours = this.defaultExpiry) {
    try {
      const decoded = this.verifyToken(existingToken);
      
      // Verificar se já expirou
      if (decoded.exp < Date.now()) {
        return { success: false, reason: 'token_expired' };
      }
      
      // Gerar novo token com expiração estendida
      return this.generateLink(decoded.userId, decoded.permissions, additionalHours);
    } catch (error) {
      return { 
        success: false, 
        reason: 'invalid_token', 
        error: error.message 
      };
    }
  }
  
  // Revogar um token (adicionando à lista de revogados)
  revokeToken(token) {
    // Em uma implementação real, armazenar em banco de dados
    // Por enquanto, apenas simulamos a revogação
    try {
      const decoded = this.verifyToken(token);
      console.log(`Token revogado para usuário ${decoded.userId}`);
      return { success: true, userId: decoded.userId };
    } catch (error) {
      return { success: false, error: error.message };
    }
  }
  
  // Métodos auxiliares para tokens seguros
  generateSecureToken(userId, permissions, expiryTime) {
    const payload = {
      userId: userId,
      permissions: permissions || [],
      exp: expiryTime,
      iat: Date.now(),
      jti: crypto.randomBytes(16).toString('hex') // ID único para o token
    };
    
    return jwt.sign(payload, this.tokenSecret, { algorithm: 'HS256' });
  }
  
  verifyToken(token) {
    return jwt.verify(token, this.tokenSecret);
  }
}

module.exports = RemoteAccessLinkGenerator;